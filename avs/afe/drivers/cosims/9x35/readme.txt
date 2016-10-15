How to run cosim test for 9x35?
1. Use file lpasshwio_devcfg_9x35.c to over-writen lpasshwio_devcfg_8974.c. Both files are under adsp_proc\avs\afe\drivers\hw\lpasshwio\devcfg\src
2. Use the below command to compile
  python build.py –o SIM           -> To create a SIM image for 8974
  python build.py –o clean         -> To clean the image

3. Run SIM test with below command. Note q6ss.cfg should from adsp_proc\avs\afe\drivers\cosims\9x35

source elite_env.sh
cd avs && qdsp6-sim --simulated_returnval --mv5c_256 ../obj/qdsp6v5_ReleaseG/dsp1.mbn --simulated_returnval --rtos ../obj/qdsp6v5_ReleaseG/osam.cfg --symfile ../build/ms/M8974AAAAAAAAQ1234_reloc.elf --symfile ../build/ms/M8974AAAAAAAAQ1234_SENSOR_reloc.elf --cosim_file ./q6ss.cfg