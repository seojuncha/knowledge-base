# Organize my workspace directory
Three different source code
- main(master)
- stable
- experiments

The experiments only be doing in the local branch.

## Key Concepts
- `git clone --bare`
- `git worktree add`

## Directories
- `/workspace/src/.repos/linux.git`
  - bare clone in here
- `/workspace/src/linux/{main,stable,exp}`
  - upstream main branch, stable branch(fixed version), and experiment local branch
- `/workspace/build/linux/{main,stable,exp}`
  - build output for each version

## Script
```bash
#!/bin/bash

cd /workspace/src/.repos
git clone --bare https://github.com/torvalds/linux.git linux.git

# Update upstream
git --git-dir=/workspace/src/.repos/linux.git fetch --all --tags

# create local experiment branch
git --git-dir=/workspace/src/.repos/linux.git branch linux-exp master

# up to date main tree
# git pull --ff-only
git --git-dir=/workspace/src/.repos/linux.git worktree add /workspace/src/linux/linux-main master
git --git-dir=/workspace/src/.repos/linux.git worktree add /workspace/src/linux/linux-stable v6.6
git --git-dir=/workspace/src/.repos/linux.git worktree add /workspace/src/linux/linux-exp linux-exp

# worktree status
git --git-dir=/workspace/src/.repos/linux.git worktree list

# Create bulid directories
mkdir -p /workspace/build/linux/{main,stable,exp}
```

## Linux build

### Prepare
```bash
sudo apt update
sudo apt install -y \
  build-essential bc bison flex libssl-dev libelf-dev \
  dwarves pahole ccache \
  libncurses-dev
```

### Build
```bash
cd /workspace/src/linux/linux-main
make O=/workspace/build/linux/main defconfig
make O=/workspace/build/linux/main -j"$(nproc)"
```

### Output
```bash
file /workspace/build/linux/main/arch/x86/boot/bzImage

# All output MUST not be in the source directory.
ls /workspace/src/linux/linux-main | grep -E "vmlinux|\.config"

# Configured file(.config) also be in build directory.
ls /workspace/build/linux/main/.config
ls /workspace/build/linux/main | head
```

