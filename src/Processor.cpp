#include "Processor.h"


Processor::Processor():params(_CONFIG_JSON) {
  raw = nullptr;
  data = nullptr;
  stft = nullptr;
  input = nullptr;
  output = nullptr;
  buf_out = nullptr;

  channels = params["CHANNEL"];
  samplerate = params["SAMPLE_RATE"];
  frame_size = params["FRAME_SIZE"];
  shift_size = params["SHIFT_SIZE"];
  reference = 0;
  
  BuildModule(channels, samplerate, frame_size, shift_size, reference);
}

Processor::~Processor() {
  ClearModule();
}

void Processor::BuildModule(int channels_, int samplerate_,int frame_size_, int shift_size_, int reference_) {
  /* release pre-allocated memory */
  ClearModule();

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

  raw = new double* [channels];
  for (int i = 0; i < channels; i++)
    raw[i] = new double[shift_size];

  data = new double* [channels];
  for (int i = 0; i < channels; i++)
    data[i]= new double[frame_size];

  buf_in = new short[channels * shift_size];
  buf_out = new short[1 * shift_size];

  stft = new STFT(channels, frame_size, shift_size);
}

void Processor::ClearModule() {
  if (stft) {
    for (int i = 0; i < channels; i++) {
      delete[] raw[i];
      delete[] data[i];
    }
    delete[] raw;
    delete[] data;
    delete[] buf_in;
    delete[] buf_out;
      
    delete stft;

    raw = nullptr;
    data = nullptr;
    stft = nullptr;
    buf_out = nullptr;
  }
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

  input->OpenFile(in_path.toStdString().c_str());
  output->NewFile(out_path.toStdString().c_str());

  int length;
  /* Process */
  while(!input->IsEOF()){
    //input->Convert2Array(raw);
    //stft->stft(raw, data);
    length = input->ReadUnit(buf_in, shift_size * channels);
    stft->stft(buf_in, length, data);

    /* Apply Algorithms here */

    stft->istft(data[0], buf_out);
    output->Append(buf_out, shift_size);
  }

  input->Finish();
  output->Finish();
  delete input;
  delete output;

//  emit(SignalReturnOutput(out_path));
  return out_path;
}