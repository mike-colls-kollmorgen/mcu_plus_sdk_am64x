Steps to patch and rebuild the EtherCAT Slave Stack Code library for "examples/industrial_comms/ethercat_slave_demo" examples
------------------------------------------------------------------------------------------------------------------------------

Examples in "examples/industrial_comms/ethercat_slave_demo" use evaluation version of stack. They will run for 1 hour only. If you want an unlimited version, you need to rebuild the Beckhoff SSC Library used by the examples.

(1) To build this library, it is necessary to get the EtherCAT Slave Stack Code (SSC). Download EtherCAT stack version 5.12 from ETG website(http://www.ethercat.org/).

(2) Generate the patched EtherCAT Slave stack code source files using any one of the below mentioned methods:

    - Copy the "*.c" and "*.h " files to "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/SlaveFiles/src/readme.txt" folder.
    - Download Windows Patch Utility from gnuwin32 sourceforge(http://gnuwin32.sourceforge.net/downlinks/patch-bin-zip.php). (Note that this is not a TI tool. See licensing information(http://savannah.gnu.org/projects/patch/) page for more details)
    - Download Dos2Unix/Unix2Dos-Text file format converters from gnuwin32 sourceforge(https://sourceforge.net/projects/dos2unix/). (Note that this is not a TI tool. See licensing information(http://www.freebsd.org/copyright/freebsd-license.html) page for more details)
    - Patch file utility(Patch.exe) and unix2dos.exe utility can be found in their bin folders.
    - Launch DOS Command prompt
    - CD to the folder "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/" which contains bkhfSsc_kunbus_ssc.patch.
    - Execute unix2dos.exe as given below:
      $(Dos2Unix/Unix2Dos-DIR)/bin/unix2dos.exe SlaveFiles\src\*
      $(Dos2Unix/Unix2Dos-DIR)/bin/unix2dos.exe bkhfSsc_kunbus_ssc.patch
    - CD to patch file utility bin folder.
    - Execute patch.exe as given below:
      patch -p1 --ignore-whitespace --input bkhfSsc_kunbus_ssc.patch
    - Above steps are for Windows. For Linux, you can use similar steps after installing patch and dos2unix utilities.
      dos2unix SlaveFiles/src/*
      dos2unix bkhfSsc_kunbus_ssc.patch
      patch -p1 --ignore-whitespace --input bkhfSsc_kunbus_ssc.patch

(3) Build the library using following step.
    - AM64x : $(MAKE) -s -f makefile.am64x ethercat_slave_bkhfSsc_r5f.ti-arm-clang
    - AM243x : $(MAKE) -s -f makefile.am243x ethercat_slave_bkhfSsc_r5f.ti-arm-clang

(4) Update the .lib file.
    - AM64x : Copy "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/lib/ethercat_slave_bkhfSsc.am64x.r5f.ti-arm-clang.release.lib" to "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/" folder.
    - AM243x EVM : Rename "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/lib/ethercat_slave_bkhfSsc.am243x.r5f.ti-arm-clang.release.lib" to "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/lib/ethercat_slave_bkhfSsc.am243x_evm.r5f.ti-arm-clang.release.lib", and copy it to "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/" folder.
    - AM243x LP : Rename "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/lib/ethercat_slave_bkhfSsc.am243x.r5f.ti-arm-clang.release.lib" to "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/lib/ethercat_slave_bkhfSsc.am243x_lp.r5f.ti-arm-clang.release.lib", and copy it to "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/" folder.

(5) Re-build the example.
