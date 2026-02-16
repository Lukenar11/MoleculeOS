const {execSync} = require("child_process");
const fs = require("fs");
const path = require("path");

const ROOT = path.join(__dirname, "..");
const BUILD_DIR = path.join(ROOT, "build");

const BOOT_SRC = path.join(ROOT, "Boot", "Stage1", "Boot.asm");
const BOOT_BIN = path.join(BUILD_DIR, "boot.bin");

const STAGE2_SRC = path.join(ROOT, "Boot", "Stage2", "OsLoader.asm");
const STAGE2_OBJ = path.join(BUILD_DIR, "OsLoader.o");

const KERNEL_CPP = path.join(ROOT, "Kernel", "src", "main.cpp");
const KERNEL_OBJ = path.join(BUILD_DIR, "kernel_main.o");

const LINKER_LD  = path.join(ROOT, "Kernel", "linker.ld");
const KERNEL_ELF = path.join(BUILD_DIR, "kernel.elf");
const KERNEL_BIN = path.join(BUILD_DIR, "kernel.bin");

const OS_IMG = path.join(BUILD_DIR, "MoleculeOS.img");

function run(cmd) {

    console.log(">", cmd);
    execSync(cmd, {stdio: "inherit"});
}

function ensure_build_dir() {

    if (!fs.existsSync(BUILD_DIR))
        fs.mkdirSync(BUILD_DIR);
}

function assemble_bootloader() {

    console.log("Assembling bootloader...");
    run(`nasm -f bin ${BOOT_SRC} -o ${BOOT_BIN}`);
}

function assemble_os_loader() {

    console.log("Assembling OsLoader...");
    run(`nasm -f elf32 ${STAGE2_SRC} -o ${STAGE2_OBJ}`);
}

function compile_kernel() {

    console.log("Compiling kernel C++...");
    run(`clang++ -target i386-pc-linux-gnu -m32 -ffreestanding -fno-exceptions -fno-rtti -c ${KERNEL_CPP} -o ${KERNEL_OBJ}`);
}

function link_kernel() {

    console.log("Linking kernel.elf...");
    run(`ld.lld -m elf_i386 -T ${LINKER_LD} ${STAGE2_OBJ} ${KERNEL_OBJ} -o ${KERNEL_ELF}`);
}

function convert_kernel() {

    console.log("Converting kernel.elf to kernel.bin...");
    run(`llvm-objcopy -O binary ${KERNEL_ELF} ${KERNEL_BIN}`);
}

function create_disk_image() {

    console.log("Creating disk image...");
    run(`dd if=/dev/zero of=${OS_IMG} bs=512 count=2880`);
}

function write_bootloader() {

    console.log("Writing bootloader to image...");
    run(`dd if=${BOOT_BIN} of=${OS_IMG} bs=512 seek=0 conv=notrunc`);
}

function write_kernel() {

    console.log("Writing kernel to image...");
    run(`dd if=${KERNEL_BIN} of=${OS_IMG} bs=512 seek=1 conv=notrunc`);
}

function run_qemu() {

    console.log("Starting QEMU...");
    run(`qemu-system-i386 -drive format=raw,file=${OS_IMG},if=floppy`);
}

ensure_build_dir();
assemble_bootloader();
assemble_os_loader();
compile_kernel();
link_kernel();
convert_kernel();
create_disk_image();
write_bootloader();
write_kernel();
run_qemu();

console.log("\nBuild complete.");
