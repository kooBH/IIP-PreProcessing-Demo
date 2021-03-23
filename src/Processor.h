#pragma once

#include <QWidget>
#include "KError.h"

#include "STFT.h"
#include "WAV.h"
#include "jsonConfig.h"
#include "RtOutput.h"
#include "align.h"

#include "MLDR/MLDR.h"
#include "MAEC/MAEC.h"



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

	RtOutput* sp;
	int soundplay_device;
	int soundplay_samplerate;
  int len_ref;
	bool isPlaying;

	MLDR* mldr;
	MAEC* maec;
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

public:
	Processor();
	~Processor();
	void BuildModule(int channels,int samplerate,int frame_size,int shift_size,int reference=0);
	void ClearModule();

	QString Process(QString path);

public slots : 
	void SlotGetAlgo(unsigned char bit);
	void SlotReference(QString reference_path);
	void SlotSoundplayInfo(int device_, int samplerate_);

	void SlotSoundPlay();
signals : 
	void SignalReturnOutput(QString);
	
};
