#include <QWidget>
#include <QPushbutton>
#include <QComboBox>
#include <QLabel.h>
#include <QString>
#include <QFileDialog>
#include <QDialog>
#include <QStackedWidget>
#include "BorderLayout.h"
#include <QHBoxLayout>
#include <QTextBrowser>
#include "KRecorder.h"
#include "KAnalysis.h"
#include "Processor.h"
#include "KWidgetASR.h"

#include <chrono>

#include "json.hpp"
using json = nlohmann::json;

class DemoControl : public QWidget {
	Q_OBJECT
private : 
	BorderLayout layout_main;

	QWidget widget_control;
		QHBoxLayout layout_control;
		QPushButton btn_start_stop;
		QComboBox combobox_algorithm;
		QLabel label_realtime;
		QCheckBox check_realtime;
		QLabel label_refernce;
		QPushButton btn_reference;

	KRecorder widget_recorder;
	QStackedWidget widget_center;
		KAnalysis widget_spectrogram;
		QWidget widget_processing;
		QVBoxLayout layout_processing;
		QTextBrowser text_processing;
	KWidgetASR widget_ASR;
	Processor processor;
	
	int idx_algo;
	unsigned char bit_algo;
	const unsigned char bit_MLDR = 0b0000'0001;
	const unsigned char bit_MAEC = 0b0000'0010;
	const unsigned char bit_AEC_BF_loopback = 0b0000'0100;

	bool isRecording;
	bool is_real_time = false;
	int device_output=0;
	int samplerate_output=48000;

	long elapsed_sec = 0;

public : 
	DemoControl();
	~DemoControl();

signals :
	void SignalToggleRecordnig();

	void SignalSetAlgo(unsigned char);
	void SignalSetReference(QString);
	void SignalSetSoundplayInfo(int, int);

	void SignalProcessBegin();
	void SignalProcessDone();

public slots:
	void SlotProcess(QString input_path, double elapsed);
	void SlotGetOutput(QString input_path);
	void SlotGetOutputs(QString output_path,QString input_path);

	void SlotToggleRecording();
	void SlotToggleProcess();

	void SlotComboAlgo(int);

	void SlotOpenReference();
	void SlotGetSoundplayInfo(int,int);

	void SlotProcessBegin();
	void SlotProcessDone();

	void SlotToggleRealtime(int state);

	void SlotTest();
};