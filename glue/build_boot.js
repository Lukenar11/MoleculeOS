const {execSync} = require("child_process");
const fs = require("fs");
const path = require("path");

// Build directorys
const BUILD_DIR = path.join(__dirname, "..", "build");
const BOOT_SRC = path.join(__dirname, "..", "Boot", "BootProcedure", "Boot.asm");
const BOOT_BIN = path.join(BUILD_DIR, "boot.bin");
const OS_IMG = path.join(BUILD_DIR, "MoleculeOS.img");

// BUILD_DIR exists?
if (!fs.existsSync(BUILD_DIR))
    fs.mkdirSync(BUILD_DIR);

console.log("Assembling bootloader...");
execSync(`nasm -f bin ${BOOT_SRC} -o ${BOOT_BIN}`, { stdio: "inherit" });

// create Disk-Image (1.44 MB Floppy)
console.log("Creating disk image...");
execSync(`dd if=/dev/zero of=${OS_IMG} bs=512 count=2880`, { stdio: "inherit" });

// write: Bootloader => Image
console.log("Writing bootloader to image...");
execSync(`dd if=${BOOT_BIN} of=${OS_IMG} conv=notrunc`, { stdio: "inherit" });

// start QEMU
console.log("Starting QEMU...");
execSync(`qemu-system-i386 -drive format=raw,file=${OS_IMG}`, { stdio: "inherit" });

console.log("\nBuild complete.");
