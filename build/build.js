const { execSync } = require("child_process");
const fs = require("fs");
const path = require("path");

// Root + Build-Paths
const ROOT = path.join(__dirname, "..");
const BUILD = path.join(ROOT, "build");
const BIN = path.join(BUILD, "bin");

// Helper
const task = (name, command) => ({ name, command });

// Sources
const OSLOADER_SRC = path.join(ROOT, "Boot", "Boot.asm");
const OSLOADER_OBJ = path.join(BIN, "loader.o");

const KERNEL_ELF = path.join(BIN, "kernel.elf");
const KERNEL_BIN = path.join(BIN, "kernel.bin");

const COMPILER_FLAGS = "-Os -ffreestanding -fno-exceptions -fno-rtti -m32 -target i386-pc-linux-gnu";

const MULTIBOOT_SRC = path.join(ROOT, "Boot", "MultibootHeader.asm");
const MULTIBOOT_OBJ = path.join(BIN, "multiboot.o");

// Build-Steps
const ASM_TASKS = [

  task("multiboot header", () =>
    `nasm -f elf32 ${MULTIBOOT_SRC} -o ${MULTIBOOT_OBJ}`),

  task("loader", () =>
    `nasm -f elf32 ${OSLOADER_SRC} -o ${OSLOADER_OBJ}`),

  task("isr", () =>
    `nasm -f elf32 ${path.join(ROOT, "Kernel", "src", "IDT", "ISR", "ISR.asm")} -o ${path.join(BIN, "isr.o")}`),

  task("loadidt", () =>
    `nasm -f elf32 ${path.join(ROOT, "Kernel", "src", "IDT", "LoadIDT.asm")} -o ${path.join(BIN, "loadidt.o")}`),
];


const C_TASKS = [
  task("kernel C runtime", () =>
    `clang ${COMPILER_FLAGS} ${path.join("@build", "c.rsp")} -o ${path.join(BIN, "string.o")}`)
];

const CPP_TASKS = [
  task("kernel C++", () =>
    `clang++ ${COMPILER_FLAGS} ${path.join("@build", "cpp.rsp")}`)
];

const OTHER_TASKS = [
  task("link kernel", () =>
    `ld.lld @build/link.rsp`),

  task("run qemu", () =>
    `qemu-system-i386 -kernel ${KERNEL_ELF}`)
];


// Core classes
class BuildTask {
  constructor(name, commandFunction) {
    this.name = name;
    this.commandFunction = commandFunction;
  }
  run() {
    console.log(`\n[${this.name}]`);
    const cmd = this.commandFunction();
    console.log(">", cmd);
    execSync(cmd, { stdio: "inherit" });
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
  ensureDirs() {
    if (!fs.existsSync(this.build)) fs.mkdirSync(this.build);
    if (!fs.existsSync(this.bin)) fs.mkdirSync(this.bin);
  }
  moveObjectsToLowercase() {
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
  runGroup(name, tasks) {
    new TaskGroup(
      name,
      tasks.map(task => new BuildTask(task.name, task.command))
    ).run();
  }
}

(function main() {
  const build = new BuildSystem(ROOT);
  build.ensureDirs();

  build.runGroup("ASM", ASM_TASKS);
  build.runGroup("C Runtime", C_TASKS);
  build.runGroup("C++ Kernel", CPP_TASKS);

  build.moveObjectsToLowercase();

  build.runGroup("Linking & Image", OTHER_TASKS);

  console.log("\nBuild complete.");
})();
