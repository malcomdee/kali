// On-disk file system format.
// Both the kernel and user programs use this header file.

#define ROOTINO  1   // root i-number
#define BSIZE 1024  // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint magic;        // Must be FSMAGIC
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

#define FSMAGIC 0x10203040

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// Definir permisos
#define PERM_READ      0x1  // Permiso de lectura
#define PERM_WRITE     0x2  // Permiso de escritura
#define PERM_IMMUTABLE 0x4  // Archivo inmutable

// Máscaras y macros para manejar 'type' y 'perm'
#define TYPE_MASK 0x00FF      // Máscara para los 8 bits inferiores (tipo)
#define PERM_MASK 0xFF00      // Máscara para los 8 bits superiores (permisos)

#define GET_TYPE(t) ((t) & TYPE_MASK)
#define GET_PERM(t) (((t) & PERM_MASK) >> 8)
#define SET_TYPE_PERM(type, perm) (((perm) << 8) | ((type) & TYPE_MASK))

// On-disk inode structure
struct dinode {
  ushort type;           // Los 8 bits inferiores para 'type', los 8 superiores para 'perm'
  ushort major;          // Major device number (T_DEVICE only)
  ushort minor;          // Minor device number (T_DEVICE only)
  ushort nlink;          // Number of links to inode in file system
  uint size;             // Size of file (bytes)
  uint addrs[NDIRECT+1]; // Data block addresses
  // int perm;           // Eliminamos este campo para no aumentar el tamaño
};

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) ((b)/BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};
