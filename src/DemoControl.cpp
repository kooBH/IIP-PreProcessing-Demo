#include "DemoControl.h"

DemoControl::DemoControl() :QWidget() {

	/* Add Widgets */	{
		layout_control.addWidget(&btn_start_stop);
		layout_control.addWidget(&combobox_algorithm);
		layout_control.addWidget(&label_refernce);
		layout_control.addWidget(&btn_reference);
		widget_control.setLayout(&layout_control);

		layout_main.addWidget(&widget_control, BorderLayout::North);
		layout_main.addWidget(&widget_recorder, BorderLayout::West);
		layout_main.addWidget(&widget_spectrogram, BorderLayout::Center);

		setLayout(&layout_main);
	}

	/* Configure Widgets */{
		setFixedSize(QSize(1400, 700));
		btn_start_stop.setText("Start");
		btn_reference.setText("Reference");

		widget_recorder.ToggleRecorderInteract(false);

		setStyleSheet("\
			QWidget{background:rgb(153, 189, 138);}\
      QLabel{background:white;border: 1px solid black;}\
      QPushButton{color:black;}\
      QComboBox{color:black;}\
      QLabel:disabled{color:gray;}\
      QPushButton:disabled{color:gray;}\
      QComboBox:disabled{color:gray;}\
      \
      ");
		combobox_algorithm.addItem("MLDR");
		combobox_algorithm.addItem("MAEC");
		combobox_algorithm.addItem("MAEC+MLDR");
	}

	/* parameters */{
		isRecording = false;
	}

  /* Connect */{
		QObject::connect(&btn_start_stop, &QPushButton::clicked, this, &DemoControl::SlotToggleRecording);

		// Record and process
		QObject::connect(this, &DemoControl::SignalToggleRecordnig, &widget_recorder, &KRecorder::SlotToggleRecording);

		QObject::connect(&widget_recorder, &KRecorder::SignalRecordFinished, this, &DemoControl::SlotProcess);
	}

}

DemoControl::~DemoControl() {

}

void DemoControl::SlotProcess(QString path) {
	printf("Process : %s\n",path.toStdString().c_str());
	QString output = processor.Process(path);
	widget_spectrogram.LoadFile(output.toStdString().c_str());
	widget_spectrogram.LoadFile(path.toStdString().c_str());
	
	/* read wav */
	/* Run Preprocess routine */
  /* Display */

}

void DemoControl::SlotGetOutput(QString path) {
	widget_spectrogram.LoadFile(path.toStdString().c_str());
}

void DemoControl::SlotToggleRecording() {
	isRecording = !isRecording;
	emit(SignalToggleRecordnig());
}
