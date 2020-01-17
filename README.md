"# Real-Time-Operating-Systems" 
Designed and implemented a light-weight messaging kernel priority processes for an ARM Cortex Processor in C.

The light-weight messaging kernel software is designed based on layers, consists of four major component. The processes and process-kernel-call software are in process-space, the kernel and ISRs are in kernel-space. The command and related data is passing through interface supervisor call (SVC). The supporting hardware of this monitor software is the TI TM4C1294NCPDT (Tiva) microcontroller which core is the ARM Cortex-M4.
