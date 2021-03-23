#include "Processor.h"



Processor::Processor():params(_CONFIG_JSON,"param") {
  data = nullptr;
  data_ref = nullptr;
  stft = nullptr;
  stft_ref = nullptr;
  input = nullptr;
  output = nullptr;
  buf_out = nullptr;
  buf_sp_ref=nullptr;
  buf_ref = nullptr;

  sp = nullptr;

  mldr = nullptr;
  maec = nullptr;

  soundplay_device = 0;
  soundplay_samplerate = 48000;
  isPlaying = false;

  channels = params["channel"];
  samplerate = params["samplerate"];
  frame_size = params["frame_size"];
  shift_size = params["shift_size"];
  reference = 0;

  bit_algorithm = 0b0000'0001;

  BuildModule(channels, samplerate, frame_size, shift_size, reference);
}

Processor::~Processor() {
  ClearModule();
  if (sp)
    delete sp;
}

void Processor::BuildModule(int channels_, int samplerate_,int frame_size_, int shift_size_, int reference_) {
  /* Setup */
  channels = channels_;
  samplerate = samplerate_;
  frame_size = frame_size_;
  shift_size = shift_size_;

  printf("Processor::BuildModule\n");
  printf("channels   : %d\n",channels);
  printf("samplrate  : %d\n",samplerate);
  printf("frame_size : %d\n",frame_size);
  printf("shift_size : %d\n",shift_size);

  data = new double* [max_channels];
  for (int i = 0; i < max_channels; i++) {
    data[i] = new double[frame_size + 2];
    memset(data[i], 0, sizeof(double) *(frame_size+2));
  }

  data_ref = new double* [max_reference];
  for (int i = 0; i < max_reference; i++) {
    data_ref[i] = new double[frame_size + 2];
    memset(data_ref[i], 0, sizeof(double) * (frame_size + 2));
  }

  buf_in = new short[max_channels * shift_size];
  buf_out = new short[1 * shift_size];

  stft = new STFT(max_channels, frame_size, shift_size);
  stft_ref = new STFT(max_reference, frame_size, shift_size);
  
  mldr = new MLDR(frame_size, max_channels);
  maec = new MAEC(frame_size, max_channels, max_reference,false);
  //saec = new StereoAEC(frame_size, channels);
}

void Processor::ClearModule() {
  for (int i = 0; i < max_channels; i++) {
    delete[] data[i];
  }
  delete[] data;
  delete[] buf_in;
  delete[] buf_out;

  delete stft;
  delete stft_ref;
  delete mldr;

  buf_in = nullptr;
  buf_out = nullptr;
  data = nullptr;
  stft = nullptr;
  stft_ref = nullptr;
  mldr = nullptr;
  maec = nullptr;

  for (int i = 0; i < max_reference; i++) {
    delete[] data_ref[i];
  }
  delete[] data_ref;
  delete[] buf_ref;

  buf_ref = nullptr;
  data_ref = nullptr;
}

QString Processor::Process(QString path_) {
  in_path = path_;

  QString temp_qstr;
  
  /* output file path as <input file>_output.wav */
  temp_qstr = in_path;
  temp_qstr.chop(4);
  out_path = temp_qstr + "_output.wav";

  /* Set I/O */
  input = new WAV();
  output = new WAV(1, samplerate,frame_size,shift_size);

  if (bit_algorithm & bit_MAEC) {
    ref->Rewind();
    if (buf_ref)delete[] buf_ref;
    buf_ref = new short[reference * shift_size];
  }

  input->OpenFile(in_path.toStdString());
  output->NewFile(out_path.toStdString());

  int length;  

  /** Sync reference using croess correlation **/
  if (bit_algorithm & bit_MAEC) {
    int len_input = input->GetNumOfSamples();
    int len_ref   = ref->GetNumOfSamples();

    int ch_input = input->GetChannels();
    int ch_ref = ref->GetChannels();

    short* tmp_buf_input  = new short[len_input*ch_input];
    short* tmp_buf_ref = new short[len_ref*ch_ref];
    memset(tmp_buf_input, 0, sizeof(short) * (len_input * ch_input));
    memset(tmp_buf_ref, 0, sizeof(short) * (len_ref * ch_ref));
   
    input->ReadUnit(tmp_buf_input, len_input*ch_input);
    ref->ReadUnit(tmp_buf_ref, len_ref*ch_ref);

    //Sync based on first channel
    for (int i = 0; i < len_ref; i++) {
          tmp_buf_ref[i] = tmp_buf_ref[ch_ref * i];
    }
    
    int len_min;
    len_min = std::min(len_input, len_ref);

    delay = align::getDelay(tmp_buf_input, tmp_buf_ref,len_min);

    ref->Rewind();
    input->Rewind();

    /*  input is early */
    if (delay > 0) {
      ref->ReadUnit(tmp_buf_ref, delay * ch_ref);
    }
    /*  reference is early */
    else {
      input->ReadUnit(tmp_buf_input, -delay * ch_input);
    }

    delete[] tmp_buf_input;
    delete[] tmp_buf_ref;
  }

  /* Process one shift each */
  while(!input->IsEOF()){
    length = input->ReadUnit(buf_in, shift_size * channels);
    stft->stft(buf_in, length, data,channels);
    
    if (bit_algorithm & bit_MLDR)
      mldr->Process(data,channels);
   
    if (bit_algorithm & bit_MAEC) {
      length = ref->ReadUnit(buf_ref, shift_size * reference);
      
      stft_ref->stft(buf_ref, length, data_ref, reference);
      maec->Process(data, data_ref, channels, reference);
      //saec->Process(data, data_ref[0], data_ref[1], data);
    }
 
    stft->istft(data[0], buf_out);
    output->Append(buf_out, shift_size);
  }

  input->Finish();
  output->Finish();

  if (bit_algorithm & bit_MLDR)
    mldr->Clear();

  if (bit_algorithm & bit_MAEC) {
    maec->Clear();
    ref->Finish();
    delete ref;
  }
  delete input;
  delete output;

//  emit(SignalReturnOutput(out_path));
  return out_path;
}

void Processor::SlotGetAlgo(unsigned char bit_) {
  bit_algorithm = bit_;
  BuildModule(channels, samplerate, frame_size, shift_size, reference);
  
}

void Processor::SlotReference(QString path) {
 // std::cout << "Processor::SlotReference : " << path.toStdString() << std::endl;
  ref_path = path;
  ref = new WAV();
  ref->OpenFile(ref_path.toStdString());
  samplerate = ref->GetSampleRate();
  reference = ref->GetChannels();

  if (reference == 0)
    KError("Reference channel is 0.");

  if (sp) delete sp;

  sp = new RtOutput(soundplay_device, reference, samplerate, soundplay_samplerate, shift_size, frame_size);
}

void Processor::SlotSoundplayInfo(int device_, int samplerate_) {
  soundplay_device = device_;
  soundplay_samplerate = samplerate_;
//  printf("Processor::SlotSoundplayInfo(%d,%d)\n", soundplay_device, soundplay_samplerate);
}

// play reference sound
void Processor::SlotSoundPlay() {
  if (!(bit_algorithm & bit_MAEC))
    return;
  if (!isPlaying) {
    ref->Rewind();
    len_ref = ref->GetSize() / 2;

    if (buf_sp_ref)delete[] buf_sp_ref;
    buf_sp_ref = new short[len_ref];
    ref->ReadUnit(buf_sp_ref, len_ref);

    sp->FullBufLoad(buf_sp_ref, len_ref);
    sp->Start();
    isPlaying = true;
  }
  else {
    sp->Stop();
    isPlaying = false;
  }
  
}
