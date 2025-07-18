/* SPDX-License-Identifier: GPL-2.0 */
/*
 *
 * Definitions for mount interface. This describes the in the kernel build 
 * linkedlist with mounted filesystems.
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 */
#ifndef _LINUX_MOUNT_H
#define _LINUX_MOUNT_H

#include <linux/types.h>
#include <asm/barrier.h>

struct super_block;
struct dentry;
struct user_namespace;
struct mnt_idmap;
struct file_system_type;
struct fs_context;
struct file;
struct path;

enum mount_flags {
	MNT_NOSUID	= 0x01,
	MNT_NODEV	= 0x02,
	MNT_NOEXEC	= 0x04,
	MNT_NOATIME	= 0x08,
	MNT_NODIRATIME	= 0x10,
	MNT_RELATIME	= 0x20,
	MNT_READONLY	= 0x40, /* does the user want this to be r/o? */
	MNT_NOSYMFOLLOW	= 0x80,

	MNT_SHRINKABLE	= 0x100,
	MNT_WRITE_HOLD	= 0x200,

	MNT_SHARED	= 0x1000, /* if the vfsmount is a shared mount */
	MNT_UNBINDABLE	= 0x2000, /* if the vfsmount is a unbindable mount */

	MNT_INTERNAL	= 0x4000,

	MNT_LOCK_ATIME		= 0x040000,
	MNT_LOCK_NOEXEC		= 0x080000,
	MNT_LOCK_NOSUID		= 0x100000,
	MNT_LOCK_NODEV		= 0x200000,
	MNT_LOCK_READONLY	= 0x400000,
	MNT_LOCKED		= 0x800000,
	MNT_DOOMED		= 0x1000000,
	MNT_SYNC_UMOUNT		= 0x2000000,
	MNT_MARKED		= 0x4000000,
	MNT_UMOUNT		= 0x8000000,

	/*
	 * MNT_SHARED_MASK is the set of flags that should be cleared when a
	 * mount becomes shared.  Currently, this is only the flag that says a
	 * mount cannot be bind mounted, since this is how we create a mount
	 * that shares events with another mount.  If you add a new MNT_*
	 * flag, consider how it interacts with shared mounts.
	 */
	MNT_SHARED_MASK	= MNT_UNBINDABLE,
	MNT_USER_SETTABLE_MASK  = MNT_NOSUID | MNT_NODEV | MNT_NOEXEC
				  | MNT_NOATIME | MNT_NODIRATIME | MNT_RELATIME
				  | MNT_READONLY | MNT_NOSYMFOLLOW,
	MNT_ATIME_MASK = MNT_NOATIME | MNT_NODIRATIME | MNT_RELATIME,

	MNT_INTERNAL_FLAGS = MNT_SHARED | MNT_WRITE_HOLD | MNT_INTERNAL |
			     MNT_DOOMED | MNT_SYNC_UMOUNT | MNT_MARKED |
			     MNT_LOCKED,
};

struct vfsmount {
	struct dentry *mnt_root;	/* root of the mounted tree */
	struct super_block *mnt_sb;	/* pointer to superblock */
	int mnt_flags;
	struct mnt_idmap *mnt_idmap;
} __randomize_layout;

static inline struct mnt_idmap *mnt_idmap(const struct vfsmount *mnt)
{
	/* Pairs with smp_store_release() in do_idmap_mount(). */
	return READ_ONCE(mnt->mnt_idmap);
}

extern int mnt_want_write(struct vfsmount *mnt);
extern int mnt_want_write_file(struct file *file);
extern void mnt_drop_write(struct vfsmount *mnt);
extern void mnt_drop_write_file(struct file *file);
extern void mntput(struct vfsmount *mnt);
extern struct vfsmount *mntget(struct vfsmount *mnt);
extern void mnt_make_shortterm(struct vfsmount *mnt);
extern struct vfsmount *mnt_clone_internal(const struct path *path);
extern bool __mnt_is_readonly(struct vfsmount *mnt);
extern bool mnt_may_suid(struct vfsmount *mnt);

extern struct vfsmount *clone_private_mount(const struct path *path);
int mnt_get_write_access(struct vfsmount *mnt);
void mnt_put_write_access(struct vfsmount *mnt);

extern struct vfsmount *fc_mount(struct fs_context *fc);
extern struct vfsmount *vfs_create_mount(struct fs_context *fc);
extern struct vfsmount *vfs_kern_mount(struct file_system_type *type,
				      int flags, const char *name,
				      void *data);
extern struct vfsmount *vfs_submount(const struct dentry *mountpoint,
				     struct file_system_type *type,
				     const char *name, void *data);

extern void mnt_set_expiry(struct vfsmount *mnt, struct list_head *expiry_list);
extern void mark_mounts_for_expiry(struct list_head *mounts);

extern bool path_is_mountpoint(const struct path *path);

extern bool our_mnt(struct vfsmount *mnt);

extern struct vfsmount *kern_mount(struct file_system_type *);
extern void kern_unmount(struct vfsmount *mnt);
extern int may_umount_tree(struct vfsmount *);
extern int may_umount(struct vfsmount *);
int do_mount(const char *, const char __user *,
		     const char *, unsigned long, void *);
extern struct vfsmount *collect_mounts(const struct path *);
extern void drop_collected_mounts(struct vfsmount *);
extern int iterate_mounts(int (*)(struct vfsmount *, void *), void *,
			  struct vfsmount *);
extern void kern_unmount_array(struct vfsmount *mnt[], unsigned int num);

extern int cifs_root_data(char **dev, char **opts);

#endif /* _LINUX_MOUNT_H */
