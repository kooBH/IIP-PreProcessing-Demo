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
		btn_reference.setEnabled(false);

		widget_recorder.ToggleRecorderInteract(false);

		setStyleSheet("\
			QWidget{background:rgb(153, 189, 138);}\
      QLabel{background:white;border: 1px solid black;}\
      QPushButton{color:black;}\
      QComboBox{color:black;background:white;}\
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
		idx_algo=0;
		bit_algo = 0b0000'0000;
	}

  /* Connect */{
		QObject::connect(&btn_start_stop, &QPushButton::clicked, this, &DemoControl::SlotToggleRecording);
		//QObject::connect(&btn_start_stop, &QPushButton::clicked, &processor,&Processor::SlotSoundPlay);

		// set algo
		QObject::connect(&combobox_algorithm, &QComboBox::currentIndexChanged, this, &DemoControl::SlotComboAlgo);
		QObject::connect(this, &DemoControl::SignalSetAlgo, &processor, &Processor::SlotGetAlgo);

		// open reference file
		QObject::connect(&btn_reference, &QPushButton::clicked, this, &DemoControl::SlotOpenReference);
		QObject::connect(this, &DemoControl::SignalSetReference, &processor,&Processor::SlotReference);
		QObject::connect(&widget_spectrogram, &KAnalysis::SignalSetSoundplayInfo, this, &DemoControl::SlotGetSoundplayInfo);
		QObject::connect(this, &DemoControl::SignalSetSoundplayInfo, &processor, &Processor::SlotSoundplayInfo);


		// Record and process
		QObject::connect(this, &DemoControl::SignalToggleRecordnig, &widget_recorder, &KRecorder::SlotToggleRecording);
		QObject::connect(&widget_recorder, &KRecorder::SignalRecordFinished, this, &DemoControl::SlotProcess);
	}
}

DemoControl::~DemoControl() {

}

void DemoControl::SlotProcess(QString path) {
	printf("Process : %s\n",path.toStdString().c_str());

	widget_spectrogram.ToggleHide();
	QString output = processor.Process(path);

	widget_spectrogram.LoadFile(output.toStdString().c_str());
	widget_spectrogram.LoadFile(path.toStdString().c_str());

	widget_spectrogram.ToggleHide();

	if (bit_algo & bit_MAEC) {
		btn_start_stop.setEnabled(false);
	}
	
	/* read wav */
	/* Run Preprocess routine */
  /* Display */
}

void DemoControl::SlotGetOutput(QString path) {
	widget_spectrogram.LoadFile(path.toStdString().c_str());
}

void DemoControl::SlotToggleRecording() {
	isRecording = !isRecording;
	processor.SlotSoundPlay();
	emit(SignalToggleRecordnig());
}

/* 
   index of algorithm

	 0 : MLDR
	 1 : MAEC
	 2 : MLDR + MAEC
*/
void DemoControl::SlotComboAlgo(int idx_) {
	idx_algo = idx_;
	bit_algo = 0b0000'0000;
	if (idx_algo == 0) {
		bit_algo|= bit_MLDR;
	}
	else if (idx_algo == 1) {
		bit_algo|= bit_MAEC;
	}
	else if (idx_algo == 2) {
		bit_algo|= bit_MLDR;
		bit_algo|= bit_MAEC;
	}
	else {
		KError("DemoControl::SlotComboAlgo::Unknown idx_algo");
	}

	if (bit_algo & bit_MAEC) {
		btn_reference.setEnabled(true);
		btn_start_stop.setEnabled(false);
	}
	else {
		btn_reference.setEnabled(false);
		btn_start_stop.setEnabled(true);
	}
	emit(SignalSetAlgo(bit_algo));
}


void  DemoControl::SlotOpenReference() {
	QString file_path;
	QFileDialog dialog;

	// file path as absolute path
	file_path = dialog.getOpenFileName(this, tr("Reference File"), ".", tr("*.wav"));

	// convert to relative path
	QDir dir_cur = QDir::currentPath();
	file_path = dir_cur.relativeFilePath(file_path);

	// Execption for empty input
	if (file_path.isEmpty())
		return;

	btn_start_stop.setEnabled(true);
	label_refernce.setText(file_path);
	emit(SignalSetReference(file_path));
}

void DemoControl::SlotGetSoundplayInfo(int device_, int samplerate_) {
	emit(SignalSetSoundplayInfo(device_, samplerate_));
}
