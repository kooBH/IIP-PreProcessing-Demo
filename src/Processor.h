#pragma once

#include <QWidget>
#include "KError.h"

#include "STFT.h"
#include "WAV.h"
#include "jsonConfig.h"
#include "RtInput.h"
#include "RtOutput.h"
#include "align.h"

#include "MLDR/MLDR.h"
#include "MAEC/MAEC.h"
#include "AEC_BF_loopback.h"

#include <thread>


class Processor : public QWidget {
	Q_OBJECT
private:
	int cnt = 0;
	int num_algo;

	STFT* stft;
	STFT* stft_ref;

	WAV* input;
	WAV* output;
	WAV* ref;

	RtInput* rt_input;
	RtOutput* sp;

	int device_input;
	int device_output;
	int samplerate_output;

	std::thread* thread_run;
	bool is_thread_run=false;

  int len_ref;
	bool isPlaying;

	MLDR* mldr;
	MAEC* maec;
	AEC_BF_loopback* aec_bf_loopback;
	int delay;

	const int max_channels = 16;
	const int max_reference = 6;
	int channels;
	int samplerate;
	int frame_size;
	int shift_size;
	int reference;

	double** data;
	double** data_ref;

	short* buf_out;
	short* buf_in;
	short* buf_sp_ref;  // buffer for reference play
	short* buf_ref;     // buffer for reference process

	QString in_path;
  QString out_path;
	QString ref_path;

	jsonConfig params;

	unsigned char bit_algorithm;
	const unsigned char bit_MLDR = 0b0000'0001;
	const unsigned char bit_MAEC = 0b0000'0010;
	const unsigned char bit_AEC_BF_loopback = 0b0000'0100;

public:
	Processor();
	~Processor();
	void BuildModule(int device, int channels,int samplerate,int frame_size,int shift_size,int reference=0);
	void ClearModule();
	
	void Run();
	void Stop();
	void Process();
	QString Process(QString path);
	void SetDeivce(int);

public slots : 
	void SlotGetAlgo(unsigned char bit);
	void SlotReference(QString reference_path);
	void SlotSoundplayInfo(int device_, int samplerate_);
	void SlotSoundPlay();
signals : 
	void SignalReturnOutput(QString);
	void SignalReturnOutputs(QString,QString);
	
};
