#pragma once

#include <QWidget>

#include "STFT.h"
#include "WAV.h"

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

	double** raw;
	double** data;
	short* buf_out;

	QString in_path;
  QString out_path;
	
public:
	Processor();
	~Processor();
	void BuildModule(int channels,int samplerate,int frame_size,int shift_size,int reference=0);
	void ClearModule();

	void Process(QString path);

public slots : 
	void SlotBuildModule(int, int, int);
	void SlotProcess(QString);

signals : 
	void SignalReturnOutput(QString);
	
};
