const {execSync} = require("child_process");
const fs = require("fs");
const path = require("path");

// Root + Build-Paths
const ROOT = path.join(__dirname, "..");
const BUILD = path.join(ROOT, "build");
const BIN = path.join(BUILD, "bin");

// Sourches
const BOOT_SRC = path.join(ROOT, "Boot", "Stage1", "Boot.asm");
const BOOT_BIN = path.join(BIN, "boot.bin");

const OSLOADER_SRC = path.join(ROOT, "Boot", "Stage2", "OsLoader.asm");
const OSLOADER_OBJ = path.join(BIN, "osloader.o");

const KERNEL_ELF = path.join(BIN, "kernel.elf");
const KERNEL_BIN = path.join(BIN, "kernel.bin");

const OS_IMG = path.join(BIN, "MoleculeOS.img");

// Build-Steps
const ASM_TASKS = [

    {
        name: "bootloader",
        cmd: () => `nasm -f bin ${BOOT_SRC} -o ${BOOT_BIN}`
    },
    {
        name: "osloader",
        cmd: () => `nasm -f elf32 ${OSLOADER_SRC} -o ${OSLOADER_OBJ}`
    },
    {
        name: "isr",
        cmd: () =>
            `nasm -f elf32 ${path.join("Kernel", "src", "IDT", "ISR.asm")} -o ${path.join(BIN, "isr.o")}`
    },
    {
        name: "loadidt",
        cmd: () =>
            `nasm -f elf32 ${path.join("Kernel", "src", "IDT", "LoadIDT.asm")} -o ${path.join(BIN, "loadidt.o")}`
    }
];

const CPP_TASK = {

    name: "kernel C++",
    cmd: () => `clang++ @${path.join("build", "cpp.rsp")}`
};

const CPP_RENAME = [

    ["main.o", "main.o"],
    ["IDT.o", "idt.o"]
];

const OTHER_TASKS = [

    {
        name: "link kernel",
        cmd: () => `ld.lld @${path.join("build", "link.rsp")}`
    },
    {
        name: "convert kernel",
        cmd: () => `llvm-objcopy -O binary ${KERNEL_ELF} ${KERNEL_BIN}`
    },
    {
        name: "create disk image",
        cmd: () => `dd if=/dev/zero of=${OS_IMG} bs=512 count=2880`
    },
    {
        name: "write bootloader",
        cmd: () => `dd if=${BOOT_BIN} of=${OS_IMG} bs=512 seek=0 conv=notrunc`
    },
    {
        name: "write kernel",
        cmd: () => `dd if=${KERNEL_BIN} of=${OS_IMG} bs=512 seek=1 conv=notrunc`
    },
    {
        name: "run qemu",
        cmd: () => `qemu-system-i386 -drive format=raw,file=${OS_IMG},if=floppy`
    }
];

// Helper
function run(cmd) {

    console.log(">", cmd);
    execSync(cmd, {stdio: "inherit"});
}

function ensure_build_dir() {

    if (!fs.existsSync(BUILD)) fs.mkdirSync(BUILD);
    if (!fs.existsSync(BIN)) fs.mkdirSync(BIN);
}

function run_task(task) {

    console.log(`\n[${task.name}]`);
    run(task.cmd());
}

// Main
(function main() {

    ensure_build_dir();

    for (const task of ASM_TASKS) run_task(task);

    run_task(CPP_TASK);

    for (const [src, dst] of CPP_RENAME) {
    
        const src_path = path.join(ROOT, src);
        const dst_path = path.join(BIN, dst);
        if (fs.existsSync(src_path))
            fs.renameSync(src_path, dst_path);
        else 
            console.error(`Missing object file: ${src_path}`);
    }

    for (const task of OTHER_TASKS) run_task(task);

    console.log("\nBuild complete.");
})();
