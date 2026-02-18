import {execSync} from "child_process";
import {fileURLToPath} from "url";
import fs from "fs";
import path from "path";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const ROOT = path.resolve(__dirname, "..");
const BUILD_DIR = path.join(ROOT, "build");

const BOOT_SRC = path.join(ROOT, "Boot", "Stage1", "Boot.asm");
const BOOT_BIN = path.join(BUILD_DIR, "boot.bin");

const STAGE2_SRC = path.join(ROOT, "Boot", "Stage2", "OSLoader.asm");
const STAGE2_OBJ = path.join(BUILD_DIR, "osloader.o");

const KERNEL_ELF = path.join(BUILD_DIR, "kernel.elf");
const KERNEL_BIN = path.join(BUILD_DIR, "kernel.bin");
const OS_IMG  = path.join(BUILD_DIR, "MoleculeOS.img");

function run(cmd) {

    console.log(">>>", cmd);
    execSync(cmd, {stdio: "inherit", cwd: ROOT});
}

function ensureBuildDir() {

    if (!fs.existsSync(BUILD_DIR))
        fs.mkdirSync(BUILD_DIR, {recursive: true});
}

function assembleBootloader() {

    console.log("\nAssembling Stage1 (bootloader)");
    run(`nasm -f bin "${BOOT_SRC}" -o "${BOOT_BIN}"`);
}

function assembleOsLoader() {

    console.log("\n Assembling Stage2 (OSLoader)");
    run(`nasm -f elf32 "${STAGE2_SRC}" -o "${STAGE2_OBJ}"`);
}

function assembleKernelAsm() {

    console.log("\nAssembling kernel ASM files");
    const commands = [

       [path.join("Kernel", "src", "IDT", "ISR.asm"), path.join("build", "isr.o")],
       [path.join("Kernel", "src", "IDT", "LoadIDT.asm"), path.join("build", "isr.o")]
    ];

    for (let i = 0; i < commands.length; i++)
        run(`nasm -f elf32 ${commands[i][0]} -o ${commands[i][1]}`);
}

function compileKernel() {

    console.log("\nCompiling C++ kernel (via cpp.rsp)");
    run(`clang++ ${path.join("@glue", "cpp.rsp")}`);

    for (const obj of ["main.o", "Idt.o"]) {

        const src = path.join(ROOT, obj);
        const dest = path.join(BUILD_DIR, obj);

        if (fs.existsSync(src))
            fs.renameSync(src, dest);
    }
}

function linkKernel() {

    console.log("\nLinking kernel.elf (via link.rsp)");
    run(`clang++ ${path.join("@glue", "cpp.rsp")}`);;
}

function convertKernel() {

    console.log("\nConverting kernel.elf → kernel.bin");
    run(`llvm-objcopy -O binary "${KERNEL_ELF}" "${KERNEL_BIN}"`);
}

function createDiskImage() {

    console.log("\nCreating floppy image");
    run(`dd if=/dev/zero of="${OS_IMG}" bs=512 count=2880`);
}

function writeBootloader() {

    console.log("\nWriting Stage1 bootloader to image");
    run(`dd if="${BOOT_BIN}" of="${OS_IMG}" bs=512 seek=0 conv=notrunc`);
}

function writeKernel() {

    console.log("\nWriting kernel.bin to image");
    run(`dd if="${KERNEL_BIN}" of="${OS_IMG}" bs=512 seek=1 conv=notrunc`);
}

function runQemu() {

    console.log("\nStarting QEMU");
    run(`qemu-system-i386 -drive format=raw,file="${OS_IMG}",if=floppy`);
}

async function main() {

    ensureBuildDir();
    assembleBootloader();
    assembleOsLoader();
    assembleKernelAsm();
    compileKernel();
    linkKernel();
    convertKernel();
    createDiskImage();
    writeBootloader();
    writeKernel();
    runQemu();
    console.log("\nBuild complete.");
}

main().catch(err => {

    console.error("Build failed:", err);
    process.exit(1);
});
