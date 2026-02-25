const {execSync} = require("child_process");
const fs = require("fs");
const path = require("path");

// Root + Build-Paths
const ROOT = path.join(__dirname, "..");
const BUILD = path.join(ROOT, "build");
const BIN = path.join(BUILD, "bin");

// Helper: define tasks compactly
const task = (name, command) => ({name, command});

// Sources
const BOOT_SRC = path.join(ROOT, "Boot", "Stage1", "Boot.asm");
const BOOT_BIN = path.join(BIN, "boot.bin");

const OSLOADER_SRC = path.join(ROOT, "Boot", "Stage2", "Loader.asm");
const OSLOADER_OBJ = path.join(BIN, "loader.o");

const KERNEL_ELF = path.join(BIN, "kernel.elf");
const KERNEL_BIN = path.join(BIN, "kernel.bin");

const OS_IMG = path.join(BIN, "MoleculeOS.img");

// Build-Steps
const ASM_TASKS = [

    task("bootloader", () => `nasm -f bin ${BOOT_SRC} -o ${BOOT_BIN}`),
    task("osloader", () => `nasm -f elf32 ${OSLOADER_SRC} -o ${OSLOADER_OBJ}`),
    task("stack", () => `nasm -f elf32 ${ROOT}/Kernel/Boot/Stack.asm -o ${BIN}/stack.o`),
    task("kernelentry", () => `nasm -f elf32 ${ROOT}/Kernel/Boot/KernelEntry.asm -o ${BIN}/kernelentry.o`),
    task("isr", () => `nasm -f elf32 ${ROOT}/Kernel/src/IDT/ISR.asm -o ${BIN}/isr.o`),
    task("loadidt", () => `nasm -f elf32 ${ROOT}/Kernel/src/IDT/LoadIDT.asm -o ${BIN}/loadidt.o`)
];

const C_TASKS = 
    [task("kernel C runtime", () => `clang @build/c.rsp -o ${BIN}/string.o`)];

const CPP_TASKS = 
    [task("kernel C++", () => `clang++ @build/cpp.rsp`)];

const OTHER_TASKS = [

    task("link kernel", () => `ld.lld @build/link.rsp`),
    task("convert kernel", () => `llvm-objcopy -O binary ${KERNEL_ELF} ${KERNEL_BIN}`),
    task("create disk image", () => `dd if=/dev/zero of=${OS_IMG} bs=512 count=2880`),
    task("write bootloader", () => `dd if=${BOOT_BIN} of=${OS_IMG} bs=512 seek=0 conv=notrunc`),
    task("write kernel", () => `dd if=${KERNEL_BIN} of=${OS_IMG} bs=512 seek=1 conv=notrunc`),
    task("run qemu", () => `qemu-system-i386 --accel tcg,thread=single -drive format=raw,file=${OS_IMG},if=floppy`)
];

// Core classes
class BuildTask {

    constructor(name, command_function) {

        this.name = name;
        this.command_function = command_function;
    }

    run() {

        console.log(`\n[${this.name}]`);

        const cmd = this.command_function();
        console.log(">", cmd);
        execSync(cmd, {stdio: "inherit"});
    }
}

class TaskGroup {

    constructor(name, tasks = []) {

        this.name = name;
        this.tasks = tasks;
    }

    run() {

        console.log(`\n=== ${this.name} ===`);
        for (const task of this.tasks) task.run();
    }
}

class BuildSystem {

    constructor(root) {

        this.root = root;
        this.build = path.join(root, "build");
        this.bin = path.join(this.build, "bin");
    }

    ensure_dirs() {

        if (!fs.existsSync(this.build)) fs.mkdirSync(this.build);
        if (!fs.existsSync(this.bin)) fs.mkdirSync(this.bin);
    }

    move_objects_to_lowercase() {

        const files = fs.readdirSync(this.root);
        for (const file of files)
            if (file.endsWith(".o")) {

                const lower = file.toLowerCase();
                fs.renameSync(
                    path.join(this.root, file),
                    path.join(this.bin, lower)
                );
            }
    }

    run_group(name, tasks) {

        new TaskGroup(
            name, 
            tasks.map(task => new BuildTask(task.name, task.command))
        ).run();
    }
}

(function main() {

    const build = new BuildSystem(ROOT);
    build.ensure_dirs();

    build.run_group("ASM", ASM_TASKS);
    build.run_group("C Runtime", C_TASKS);
    build.run_group("C++ Kernel", CPP_TASKS);

    build.move_objects_to_lowercase();

    build.run_group("Linking & Image", OTHER_TASKS);

    console.log("\nBuild complete.");
})();
