#include "AS.hpp"


Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);
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
	p->addModel(modelMixer8ch);
	p->addModel(modelMonoVUmeter);
	p->addModel(modelStereoVUmeter);
	p->addModel(modelSteps);
	p->addModel(modelLaunchGate);
	p->addModel(modelKillGate);
	p->addModel(modelFlow);
	p->addModel(modelSignalDelay);
	p->addModel(modelMultiple2_5);
	p->addModel(modelMerge2_5);
	p->addModel(modelTriggersMKI);
	p->addModel(modelTriggersMKII);
	p->addModel(modelTriggersMKIII);
	p->addModel(modelBPMCalc);
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

	p->addModel(modelTremoloFx);
	p->addModel(modelTremoloStereoFx);
	
	p->addModel(modelWaveShaper);
	//BLANK PANELS
	p->addModel(modelBlankPanel4);
	p->addModel(modelBlankPanel6);
	p->addModel(modelBlankPanel8);
	p->addModel(modelBlankPanelSpecial);

}
