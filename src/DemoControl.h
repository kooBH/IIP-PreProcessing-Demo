#include <QWidget>
#include <QPushbutton>
#include <QComboBox>
#include <QLabel.h>
#include <QString>
#include <QFileDialog>

#include "BorderLayout.h"
#include <QHBoxLayout>

#include "KRecorder.h"
#include "KAnalysis.h"
#include "Processor.h"

class DemoControl : public QWidget {
	Q_OBJECT
private : 
	BorderLayout layout_main;

	QWidget widget_control;
		QHBoxLayout layout_control;
		QPushButton btn_start_stop;
		QComboBox combobox_algorithm;
		QLabel label_refernce;
		QPushButton btn_reference;

	KRecorder widget_recorder;
	KAnalysis widget_spectrogram;
	Processor processor;
	
	int idx_algo;
	unsigned char bit_algo;
	unsigned char bit_MLDR = 0b0000'0001;
	unsigned char bit_MAEC = 0b0000'0010;


	bool isRecording;

public : 
	DemoControl();
	~DemoControl();

signals :
	void SignalToggleRecordnig();
	void SignalSetAlgo(unsigned char);
	void SignalSetReference(QString);
	void SignalSetSoundplayInfo(int, int);

public slots:
	void SlotProcess(QString input_path);
	void SlotGetOutput(QString input_path);

	void SlotToggleRecording();

	void SlotComboAlgo(int);

	void SlotOpenReference();
	void SlotGetSoundplayInfo(int,int);
};