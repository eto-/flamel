if [ -z `which ups` ]; then
  export CAENDIGITIZER_INC=/usr/local/caen/CAENDigitizer_2.6.5/include
  export CAENDIGITIZER_LIB=/usr/local/caen/CAENDigitizer_2.6.5/lib/x86_64
  export CAENCOMM_LIB=/usr/local/caen/CAENComm-1.2/lib/x64
  export CAENVME_LIB=/usr/local/caen/CAENVMELib-2.41/lib/x64
  export BOOST_INC=/usr/include
  export BOOST_LIB=/usr/lib/x86_64-linux-gnu
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CAENDIGITIZER_LIB:$CAENCOMM_LIB:$CAENVME_LIB
else
  echo "using FNAL setup"
  setup gcc v4_9_1 
  setup root v5_34_12 -q e4:prof
  setup boost v1_56_0 -q e6:prof
  setup caendigitizer v2_6_5 -q e4:prof
fi
