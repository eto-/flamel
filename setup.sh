if [ -z `which ups` ]; then
  export CAENDIGITIZER_INC=/usr/local/caen/CAENDigitizer/include
  export CAENDIGITIZER_LIB=/usr/local/caen/CAENDigitizer/lib/x86_64
  export CAENCOMM_LIB=/usr/local/caen/CAENComm/lib/x64
  export CAENVME_LIB=/usr/local/caen/CAENVMELib/lib/x64
  export BOOST_INC=/usr/include
  export BOOST_LIB=/usr/lib/x86_64-linux-gnu
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CAENDIGITIZER_LIB:$CAENCOMM_LIB:$CAENVME_LIB
  choose_root root_v6.08.06
else
  echo "using FNAL setup"
  setup gcc v4_9_2 
  setup root v5_34_30 -q e7:prof
  setup boost v1_57_0 -q e7:prof
  setup caendigitizer v2_6_5 -q e4:prof
  choose_root root_v6.08.06
fi
