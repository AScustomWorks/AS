#include "AS.hpp"


Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;
	//OSCILLATORS
	p->addModel(modelSineOsc);
	p->addModel(modelSawOsc);

	//TOOLS
	p->addModel(modelADSR);
	p->addModel(modelVCA);
	p->addModel(modelQuadVCA);
	p->addModel(modelTriLFO);
	p->addModel(modelAtNuVrTr);
	p->addModel(modelBPMClock);
	p->addModel(modelSEQ16);
	p->addModel(modelMixer2ch);
	p->addModel(modelMixer4ch);
	p->addModel(modelMixer8ch);
	p->addModel(modelMonoVUmeter);
	p->addModel(modelStereoVUmeter);
	p->addModel(modelMultiple2_5);
	p->addModel(modelMerge2_5);

	p->addModel(modelSteps);
	p->addModel(modelLaunchGate);
	p->addModel(modelKillGate);

	p->addModel(modelFlow);
	
	p->addModel(modelSignalDelay);
	
	p->addModel(modelTriggersMKI);
	p->addModel(modelTriggersMKII);
	
	p->addModel(modelTriggersMKIII);
	p->addModel(modelBPMCalc);
	p->addModel(modelBPMCalc2);
	
	p->addModel(modelCv2T);
	p->addModel(modelZeroCV2T);
	p->addModel(modelReScale);

	//EFFECTS	
	p->addModel(modelDelayPlusFx);
	p->addModel(modelDelayPlusStereoFx);
	
	p->addModel(modelPhaserFx);

	p->addModel(modelReverbFx);
	p->addModel(modelReverbStereoFx);

	p->addModel(modelSuperDriveFx);
	p->addModel(modelSuperDriveStereoFx);

	p->addModel(modelTremoloFx);
	p->addModel(modelTremoloStereoFx);
	
	p->addModel(modelWaveShaper);
	p->addModel(modelWaveShaperStereo);
	//BLANK PANELS
	p->addModel(modelBlankPanel4);
	p->addModel(modelBlankPanel6);
	p->addModel(modelBlankPanel8);
	p->addModel(modelBlankPanelSpecial);


}
