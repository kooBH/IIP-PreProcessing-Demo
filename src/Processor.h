#pragma once

#include <QWidget>

#include "STFT.h"
#include "WAV.h"
#include "Config.h"

class Processor : public QWidget {
	Q_OBJECT
private:
	STFT* stft;
	WAV* input;
	WAV* output;

	int channels;
	int samplerate;
	int frame_size;
	int shift_size;
	int reference;

	double** raw;
	double** data;
	short* buf_out;
	short* buf_in;

	QString in_path;
  QString out_path;

	ConfigParam params;
	
public:
	Processor();
	~Processor();
	void BuildModule(int channels,int samplerate,int frame_size,int shift_size,int reference=0);
	void ClearModule();

	QString Process(QString path);

public slots : 

signals : 
	void SignalReturnOutput(QString);
	
};
