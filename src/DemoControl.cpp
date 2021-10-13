#include "DemoControl.h"

DemoControl::DemoControl() :QWidget(){

	/* Add Widgets */	{
		layout_control.addWidget(&btn_start_stop);
		layout_control.addWidget(&combobox_algorithm);
		//layout_control.addWidget(&label_realtime);
		//layout_control.addWidget(&check_realtime);
		layout_control.addWidget(&label_refernce);
		layout_control.addWidget(&btn_reference);
		widget_control.setLayout(&layout_control);

		layout_main.addWidget(&widget_control, BorderLayout::North);
		layout_main.addWidget(&widget_recorder, BorderLayout::West);
		layout_main.addWidget(&widget_center, BorderLayout::Center);
			widget_center.addWidget(&widget_spectrogram);
			widget_center.addWidget(&widget_processing);
			widget_processing.setLayout(&layout_processing);
			layout_processing.addWidget(&text_processing);
		layout_main.addWidget(&widget_ASR, BorderLayout::South);

		setLayout(&layout_main);
	}

	/* Configure Widgets */{
		setFixedSize(QSize(1600, 900));
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
			QTextBrowser{background:rgb(246, 228, 247);color:black;}\
      \
      ");
		combobox_algorithm.addItem("MLDR");
		combobox_algorithm.addItem("MAEC");
		combobox_algorithm.addItem("MAEC->MLDR");
		combobox_algorithm.addItem("AEC_BF_loopback");

		label_realtime.setText("Real Time");
		label_realtime.setFixedWidth(100);


		QFont font_processing;
		font_processing.setPointSizeF(16);
		text_processing.setFont(font_processing);

		std::ifstream ifs("../private/config.json");
		json j = json::parse(ifs);
		widget_ASR.Init(j["key"].get<std::string>(), j["language"].get<std::string>());
		widget_ASR.label_result.setFixedHeight(200);

	}

	/* parameters */{
		isRecording = false;
		idx_algo=0;
		bit_algo = 0b0000'0001;
	}

  /* Connect */{


	  // batch routine
		//QObject::connect(&btn_start_stop, &QPushButton::clicked, this, &DemoControl::SlotToggleRecording);

		// realtime routine 
		QObject::connect(&btn_start_stop, &QPushButton::clicked, this, &DemoControl::SlotToggleProcess);
		//QObject::connect(&btn_start_stop, &QPushButton::clicked, &processor,&Processor::SlotSoundPlay);
		QObject::connect(&processor, &Processor::SignalReturnOutputs, this, &DemoControl::SlotGetOutputs);

		// set algo
		QObject::connect(&combobox_algorithm, &QComboBox::currentIndexChanged, this, &DemoControl::SlotComboAlgo);
		QObject::connect(this, &DemoControl::SignalSetAlgo, &processor, &Processor::SlotGetAlgo);

		QObject::connect(&check_realtime, &QCheckBox::stateChanged, this, &DemoControl::SlotToggleRealtime);

		// open reference file
		QObject::connect(&btn_reference, &QPushButton::clicked, this, &DemoControl::SlotOpenReference);
		QObject::connect(this, &DemoControl::SignalSetReference, &processor,&Processor::SlotReference);
		QObject::connect(&widget_spectrogram, &KAnalysis::SignalSetSoundplayInfo, this, &DemoControl::SlotGetSoundplayInfo);
		QObject::connect(this, &DemoControl::SignalSetSoundplayInfo, &processor, &Processor::SlotSoundplayInfo);


		// Record and process
		QObject::connect(this, &DemoControl::SignalToggleRecordnig, &widget_recorder, &KRecorder::SlotToggleRecording);
		QObject::connect(&widget_recorder, &KRecorder::SignalRecordFinished, this, &DemoControl::SlotProcess);
		QObject::connect(this, &DemoControl::SignalProcessBegin, this, &DemoControl::SlotProcessBegin);
		QObject::connect(this, &DemoControl::SignalProcessDone, this, &DemoControl::SlotProcessDone);

	}


}

DemoControl::~DemoControl() {

}

void DemoControl::SlotProcess(QString path,double input_elapsed) {

	text_processing.setText("");
	this->setEnabled(false);
	text_processing.append("Input : " + QString::number(input_elapsed) + " sec");
	emit(SignalProcessBegin());
	auto begin = std::chrono::high_resolution_clock::now();

	QString output = processor.Process(path);

	auto elapsed = std::chrono::high_resolution_clock::now() - begin;
	elapsed_sec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
	text_processing.append("Process : "+QString::number((double)elapsed_sec/1000) + " sec");
	text_processing.append("Drawing Spectrograms...");
	this->repaint();
	widget_spectrogram.LoadFile(output.toStdString().c_str());
	widget_spectrogram.LoadFile(path.toStdString().c_str());
	emit(SignalProcessDone());
	widget_ASR.Load(output.toStdString());

	this->setEnabled(true);
	if (bit_algo & bit_MAEC) {
		btn_start_stop.setEnabled(false);
		label_refernce.setText("NULL");
	}
	
	/* read wav */
	/* Run Preprocess routine */
  /* Display */
}

void DemoControl::SlotGetOutput(QString path) {
	widget_spectrogram.LoadFile(path.toStdString().c_str());
}
void DemoControl::SlotGetOutputs(QString path_out, QString path_in) {
	widget_spectrogram.LoadFile(path_out.toStdString().c_str());
	widget_spectrogram.LoadFile(path_in.toStdString().c_str());
	widget_ASR.Load(path_out.toStdString());
}

// Toggle For Batch Process
void DemoControl::SlotToggleRecording() {
	isRecording = !isRecording;
	if (isRecording)
		btn_start_stop.setText("STOP");
	else
		btn_start_stop.setText("START");
	processor.SlotSoundPlay();
	emit(SignalToggleRecordnig());
}

// Toggle For Real Time Process
void DemoControl::SlotToggleProcess() {
	printf("SlotToggleProcess %d\n",isRecording);
	isRecording = !isRecording;

	// Run
	if (isRecording) {
		processor.Process();
		btn_start_stop.setText("STOP");
	}
	// Stop
	else {
		processor.Stop();
		btn_start_stop.setText("START");
	}
	// For AEC reference Play
	processor.SlotSoundPlay();
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
	// AEC_BF_loopback
	else if (idx_algo == 3) {
		bit_algo |= bit_AEC_BF_loopback;
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
	device_output = device_;
	samplerate_output = samplerate_;
	emit(SignalSetSoundplayInfo(device_, samplerate_));
}

void DemoControl::SlotTest() {

}

void DemoControl::SlotProcessBegin() {
	printf("DemoControl::SlotProcessBegin()\n");
	widget_center.setCurrentIndex(1);
	this->repaint();
}

void DemoControl::SlotProcessDone() {
	printf("DemoControl::SlotProcessDone()\n");
	widget_center.setCurrentIndex(0);
	this->repaint();
}


void DemoControl::SlotToggleRealtime(int state) {
	if (state == 2) {
		is_real_time = true;
	}
	else {
		is_real_time = false;
	}
}
