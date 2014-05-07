--- ./fuse_module/fuse_vnops.c.orig	2012-09-19 06:04:10.512413490 +0200
+++ ./fuse_module/fuse_vnops.c	2012-09-19 06:20:44.949613892 +0200
@@ -175,6 +175,11 @@
 /* file ops */
 static fo_close_t fuse_close_f;
 
+#if __FreeBSD_version > 900040
+static fo_chmod_t fuse_chmod_dummy;
+static fo_chown_t fuse_chown_dummy;
+#endif
+
 /* vnode ops */
 static vop_getattr_t  fuse_getattr;
 static vop_reclaim_t  fuse_reclaim;
@@ -219,5 +219,9 @@
	.fo_kqfilter = NULL,
	.fo_stat     = NULL,
	.fo_close    = fuse_close_f,
+#if __FreeBSD_version > 900040
+	.fo_chmod     = fuse_chmod_dummy,
+	.fo_chown     = fuse_chown_dummy,
+#endif
	.fo_flags    = DFLAG_PASSABLE | DFLAG_SEEKABLE
 };
@@ -799,8 +799,11 @@
 	struct vnode *vp = ap->a_vp;
 	struct vattr *vap = ap->a_vap;
 	struct ucred *cred = ap->a_cred;
+#if VOP_GETATTR_TAKES_THREAD
 	struct thread *td = ap->a_td;
-
+#else
+	struct thread *td = curthread;
+#endif
 	struct fuse_dispatcher fdi;
 	struct timespec uptsp;
 	int err = 0;
@@ -871,7 +874,11 @@
 fuse_access(ap)
 	struct vop_access_args /* {
 		struct vnode *a_vp;
+#if VOP_ACCESS_TAKES_ACCMODE_T
+		accmode_t a_accmode;
+#else
 		int a_mode;
+#endif
 		struct ucred *a_cred;
 		struct thread *a_td;
 	} */ *ap;
@@ -886,7 +893,13 @@
 	else
 		facp.facc_flags |= FACCESS_DO_ACCESS;
 
-	return fuse_access_i(vp, ap->a_mode, ap->a_cred, ap->a_td, &facp);
+	return fuse_access_i(vp,
+#if VOP_ACCESS_TAKES_ACCMODE_T
+	    ap->a_accmode,
+#else
+	    ap->a_mode,
+#endif
+	    ap->a_cred, ap->a_td, &facp);
 }
 
 /*
@@ -946,7 +959,11 @@
 		/* We are to do the check in-kernel */
 
 		if (! (facp->facc_flags & FACCESS_VA_VALID)) {
-			err = VOP_GETATTR(vp, VTOVA(vp), cred, td);
+			err = VOP_GETATTR(vp, VTOVA(vp), cred
+#if VOP_GETATTR_TAKES_THREAD
+			    , td
+#endif
+			    );
 			if (err)
 				return (err);
 			facp->facc_flags |= FACCESS_VA_VALID;
@@ -1544,7 +1561,7 @@
 	struct fuse_vnode_data *fvdat = VTOFUD(vp);
 	uint64_t parentid = fvdat->parent_nid;
 	struct componentname *cnp = fvdat->germcnp;
-	struct fuse_open_in *foi;
+	struct fuse_create_in *foi;
 	struct fuse_entry_out *feo;
 	struct fuse_mknod_in fmni;
 	int err;
@@ -1929,7 +1946,11 @@
 		 * It will not invalidate pages which are dirty, locked, under
 		 * writeback or mapped into pagetables.") 
 		 */
+#if VOP_GETATTR_TAKES_THREAD
 		err = vinvalbuf(vp, 0, td, PCATCH, 0);
+#else
+		err = vinvalbuf(vp, 0, PCATCH, 0);
+#endif
 		fufh->flags |= FOPEN_KEEP_CACHE;
 	}
 
@@ -3005,8 +3026,11 @@
 	struct vattr *vap = ap->a_vap;
 	struct vnode *vp = ap->a_vp;
 	struct ucred *cred = ap->a_cred;
+#if VOP_GETATTR_TAKES_THREAD
 	struct thread *td = ap->a_td;
-
+#else
+	struct thread *td = curthread;
+#endif
 	int err = 0;
 	struct fuse_dispatcher fdi;
 	struct fuse_setattr_in *fsai;
@@ -3659,3 +3668,17 @@
        return (0);
 }
 #endif
+
+#if __FreeBSD_version > 900040
+static int
+fuse_chmod_dummy(struct file *fp, mode_t mode,
+                struct ucred *active_cred, struct thread *td) {
+  return (ENOSYS);
+}
+
+static int
+fuse_chown_dummy(struct file *fp, uid_t uid, gid_t gid,
+                struct ucred *active_cred, struct thread *td) {
+  return (ENOSYS);
+}
+#endif
