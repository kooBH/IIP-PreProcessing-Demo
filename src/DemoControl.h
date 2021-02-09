#include <QWidget>
#include <QPushbutton>
#include <QComboBox>
#include <QLabel.h>
#include <QString>

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

	bool isRecording;

public : 
	DemoControl();
	~DemoControl();

signals :
	void SignalToggleRecordnig();

public slots:
	void SlotProcess(QString input_path);
	void SlotGetOutput(QString input_path);

	void SlotToggleRecording();


};