--- iavf_osdep.c.orig	2024-09-06 05:36:15 UTC
+++ iavf_osdep.c
@@ -41,7 +41,7 @@
  */
 #include <machine/stdarg.h>
 
-#include "iavf.h"
+#include "iavf_iflib.h"
 
 /********************************************************************
  * Manage DMA'able memory.
@@ -321,7 +321,7 @@
 }
 
 /**
- * rd32 - Read a 32bit hardware register value
+ * iavf_rd32 - Read a 32bit hardware register value
  * @hw: the private hardware structure
  * @reg: register address to read
  *
@@ -330,7 +330,7 @@
  * @returns the value of the 32bit register.
  */
 inline uint32_t
-rd32(struct iavf_hw *hw, uint32_t reg)
+iavf_rd32(struct iavf_hw *hw, uint32_t reg)
 {
 	struct iavf_osdep *osdep = (struct iavf_osdep *)hw->back;
 
@@ -343,7 +343,7 @@
 }
 
 /**
- * wr32 - Write a 32bit hardware register
+ * iavf_wr32 - Write a 32bit hardware register
  * @hw: the private hardware structure
  * @reg: the register address to write to
  * @val: the 32bit value to write
@@ -351,7 +351,7 @@
  * Write the specified 32bit value to a register address in BAR0.
  */
 inline void
-wr32(struct iavf_hw *hw, uint32_t reg, uint32_t val)
+iavf_wr32(struct iavf_hw *hw, uint32_t reg, uint32_t val)
 {
 	struct iavf_osdep *osdep = (struct iavf_osdep *)hw->back;
 
