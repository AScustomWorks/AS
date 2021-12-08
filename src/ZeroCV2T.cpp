//**************************************************************************************
//CV to Trigger convenrter module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"

struct ZeroCV2T : Module {
	enum ParamIds {
		TRIG_SWITCH_1,
		TRIG_SWITCH_2,
		TRIG_SWITCH_3,
		TRIG_SWITCH_4,
		NUM_PARAMS
	};
	enum InputIds {
		CV_IN_1,
		CV_IN_2,
		CV_IN_3,
		CV_IN_4,
		NUM_INPUTS
	};
	enum OutputIds {
		TRIG_OUT_1,
		TRIG_OUT_2,
		TRIG_OUT_3,
		TRIG_OUT_4,
		NUM_OUTPUTS
	};
	enum LightIds {
		TRIG_LED_1,
		TRIG_LED_2,
		TRIG_LED_3,
		TRIG_LED_4,
		NUM_LIGHTS
	};

	dsp::SchmittTrigger trig_1, trig_2, trig_3, trig_4;

	dsp::PulseGenerator trigPulse1[16], trigPulse2[16], trigPulse3[16], trigPulse4[16];

	float trigger_length = 0.001f;

	const float lightLambda = 0.075f;
	float trigLight1 = 0.0f;
	float trigLight2 = 0.0f;
	float trigLight3 = 0.0f;
	float trigLight4 = 0.0f;

	bool cv_1_engaged[16] = {false};
	bool cv_2_engaged[16] = {false};
	bool cv_3_engaged[16] = {false};
	bool cv_4_engaged[16] = {false};

	float trigger_treshold = 0.0005f;

	ZeroCV2T() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
/* 		configParam(ZeroCV2T::TRIG_SWITCH_1 , 0.0f, 1.0f, 0.0f, "Port 1 Trigger");
		configParam(ZeroCV2T::TRIG_SWITCH_2 , 0.0f, 1.0f, 0.0f, "Port 2 Trigger");
		configParam(ZeroCV2T::TRIG_SWITCH_3 , 0.0f, 1.0f, 0.0f, "Port 3 Trigger");
		configParam(ZeroCV2T::TRIG_SWITCH_4 , 0.0f, 1.0f, 0.0f, "Port 4 Trigger"); */

		//New in V2, config temporary buttons info without displaying values
		configButton(TRIG_SWITCH_1, "Port 1 Trigger");
		configButton(TRIG_SWITCH_2, "Port 2 Trigger");
		configButton(TRIG_SWITCH_3, "Port 3 Trigger");
		configButton(TRIG_SWITCH_4, "Port 4 Trigger");

		//new V2, port labels
		//inputs
		configInput(CV_IN_1, "Port 1 CV");
		configInput(CV_IN_2, "Port 2 CV");
		configInput(CV_IN_3, "Port 3 CV");
		configInput(CV_IN_4, "Port 4 CV");
		//Outputs
		configOutput(TRIG_OUT_1, "Port 1 Trigger");
		configOutput(TRIG_OUT_2, "Port 2 Trigger");
		configOutput(TRIG_OUT_3, "Port 3 Trigger");
		configOutput(TRIG_OUT_4, "Port 4 Trigger");

	}

	void process(const ProcessArgs &args) override {

		//CV TRIG 1
		if (inputs[CV_IN_1].getChannels() == 0) {
			outputs[TRIG_OUT_1].setChannels(1);

			if ( trig_1.process( params[TRIG_SWITCH_1].getValue() ) ) {
				trigLight1 = 1.0;
				trigPulse1[0].trigger( trigger_length );
			}

			trigLight1 -= trigLight1 / lightLambda / args.sampleRate;
			lights[TRIG_LED_1].value = trigLight1;
			outputs[TRIG_OUT_1].setVoltage(trigPulse1[0].process(args.sampleTime) ? 10.f : 0.f);
		} else {
			outputs[TRIG_OUT_1].setChannels(inputs[CV_IN_1].getChannels());

			for (int i = 0; i < inputs[CV_IN_1].getChannels(); i++) {
				if ( trig_1.process( params[TRIG_SWITCH_1].getValue() ) ) {
					trigLight1 = 1.0;
					for (int j = 0; j < inputs[CV_IN_1].getChannels(); j++) {
						trigPulse1[j].trigger( trigger_length );
					}
				}

				if ( fabs( inputs[CV_IN_1].getVoltage(i) ) < trigger_treshold ){
					if(!cv_1_engaged[i]){
						cv_1_engaged[i] = true;
						trigLight1 = 1.0;
						trigPulse1[i].trigger( trigger_length );
						// send trigger
					}
				} else {
					if ( fabs( inputs[CV_IN_1].getVoltage(i) ) > trigger_treshold ) {
						// reenable trigger
						cv_1_engaged[i] = false;
					}
				}

				trigLight1 -= trigLight1 / lightLambda / args.sampleRate;
				lights[TRIG_LED_1].value = trigLight1;
				outputs[TRIG_OUT_1].setVoltage(trigPulse1[i].process(args.sampleTime) ? 10.0f : 0.0f , i);
			}
		}

		//CV 2 TRIG 2
		if (inputs[CV_IN_2].getChannels() == 0) {
			outputs[TRIG_OUT_2].setChannels(1);

			if ( trig_2.process( params[TRIG_SWITCH_2].getValue() ) ) {
				trigLight2 = 1.0;
				trigPulse2[0].trigger( trigger_length );
			}

			trigLight2 -= trigLight2 / lightLambda / args.sampleRate;
			lights[TRIG_LED_2].value = trigLight2;
			outputs[TRIG_OUT_2].setVoltage(trigPulse2[0].process(args.sampleTime) ? 10.f : 0.f);
		} else {
			outputs[TRIG_OUT_2].setChannels(inputs[CV_IN_2].getChannels());

			for (int i = 0; i < inputs[CV_IN_2].getChannels(); i++) {
				if ( trig_2.process( params[TRIG_SWITCH_2].getValue() ) ) {
					trigLight2 = 1.0;
					for (int j = 0; j < inputs[CV_IN_2].getChannels(); j++) {
						trigPulse2[j].trigger( trigger_length );
					}
				}

				if ( fabs( inputs[CV_IN_2].getVoltage(i) ) < trigger_treshold ){
					if(!cv_2_engaged[i]){
						cv_2_engaged[i] = true;
						trigLight2 = 1.0;
						trigPulse2[i].trigger( trigger_length );
						// send trigger
					}
				} else {
					if ( fabs( inputs[CV_IN_2].getVoltage(i) ) > trigger_treshold ) {
						// reenable trigger
						cv_2_engaged[i] = false;
					}
				}

				trigLight2 -= trigLight2 / lightLambda / args.sampleRate;
				lights[TRIG_LED_2].value = trigLight2;
				outputs[TRIG_OUT_2].setVoltage(trigPulse2[i].process(args.sampleTime) ? 10.0f : 0.0f , i);
			}
		}

		//CV 2 TRIG 3
		if (inputs[CV_IN_3].getChannels() == 0) {
			outputs[TRIG_OUT_3].setChannels(1);

			if ( trig_3.process( params[TRIG_SWITCH_3].getValue() ) ) {
				trigLight3 = 1.0;
				trigPulse3[0].trigger( trigger_length );
			}

			trigLight3 -= trigLight3 / lightLambda / args.sampleRate;
			lights[TRIG_LED_3].value = trigLight3;
			outputs[TRIG_OUT_3].setVoltage(trigPulse3[0].process(args.sampleTime) ? 10.f : 0.f);
		} else {
			outputs[TRIG_OUT_3].setChannels(inputs[CV_IN_3].getChannels());

			for (int i = 0; i < inputs[CV_IN_3].getChannels(); i++) {
				if ( trig_3.process( params[TRIG_SWITCH_3].getValue() ) ) {
					trigLight3 = 1.0;
					for (int j = 0; j < inputs[CV_IN_3].getChannels(); j++) {
						trigPulse3[j].trigger( trigger_length );
					}
				}

				if ( fabs( inputs[CV_IN_3].getVoltage(i) ) < trigger_treshold ){
					if(!cv_3_engaged[i]){
						cv_3_engaged[i] = true;
						trigLight3 = 1.0;
						trigPulse3[i].trigger( trigger_length );
						// send trigger
					}
				} else {
					if ( fabs( inputs[CV_IN_3].getVoltage(i) ) > trigger_treshold ) {
						// reenable trigger
						cv_3_engaged[i] = false;
					}
				}

				trigLight3 -= trigLight3 / lightLambda / args.sampleRate;
				lights[TRIG_LED_3].value = trigLight3;
				outputs[TRIG_OUT_3].setVoltage(trigPulse3[i].process(args.sampleTime) ? 10.0f : 0.0f , i);
			}
		}

		//CV 2 TRIG 4
		if (inputs[CV_IN_4].getChannels() == 0) {
			outputs[TRIG_OUT_4].setChannels(1);

			if ( trig_4.process( params[TRIG_SWITCH_4].getValue() ) ) {
				trigLight4 = 1.0;
				trigPulse4[0].trigger( trigger_length );
			}

			trigLight4 -= trigLight4 / lightLambda / args.sampleRate;
			lights[TRIG_LED_4].value = trigLight4;
			outputs[TRIG_OUT_4].setVoltage(trigPulse4[0].process(args.sampleTime) ? 10.f : 0.f);
		} else {
			outputs[TRIG_OUT_4].setChannels(inputs[CV_IN_4].getChannels());

			for (int i = 0; i < inputs[CV_IN_4].getChannels(); i++) {
				if ( trig_4.process( params[TRIG_SWITCH_4].getValue() ) ) {
					trigLight4 = 1.0;
					for (int j = 0; j < inputs[CV_IN_4].getChannels(); j++) {
						trigPulse4[j].trigger( trigger_length );
					}
				}

				if ( fabs( inputs[CV_IN_4].getVoltage(i) ) < trigger_treshold ){
					if(!cv_4_engaged[i]){
						cv_4_engaged[i] = true;
						trigLight4 = 1.0;
						trigPulse4[i].trigger( trigger_length );
						// send trigger
					}
				} else {
					if ( fabs( inputs[CV_IN_4].getVoltage(i) ) > trigger_treshold ) {
						// reenable trigger
						cv_4_engaged[i] = false;
					}
				}

				trigLight4 -= trigLight4 / lightLambda / args.sampleRate;
				lights[TRIG_LED_4].value = trigLight4;
				outputs[TRIG_OUT_4].setVoltage(trigPulse4[i].process(args.sampleTime) ? 10.0f : 0.0f , i);
			}
		}


	}


};


struct ZeroCV2TWidget : ModuleWidget {

	ZeroCV2TWidget(ZeroCV2T *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ZeroCV2T.svg")));

		//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
		addChild(createWidget<as_HexScrew>(Vec(0, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const int gp_offset = 75;
		//CV 2 TRIG 1
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101), module, ZeroCV2T::TRIG_SWITCH_1 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2), module, ZeroCV2T::TRIG_LED_1));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60), module, ZeroCV2T::CV_IN_1));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100), module, ZeroCV2T::TRIG_OUT_1));
		//CV 2 TRIG 2
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101+gp_offset*1), module, ZeroCV2T::TRIG_SWITCH_2 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2+gp_offset*1), module, ZeroCV2T::TRIG_LED_2));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60+gp_offset*1), module, ZeroCV2T::CV_IN_2));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100+gp_offset*1), module, ZeroCV2T::TRIG_OUT_2));
		//CV 2 TRIG 3
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101+gp_offset*2), module, ZeroCV2T::TRIG_SWITCH_3 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2+gp_offset*2), module, ZeroCV2T::TRIG_LED_3));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60+gp_offset*2), module, ZeroCV2T::CV_IN_3));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100+gp_offset*2), module, ZeroCV2T::TRIG_OUT_3));
		//CV 2 TRIG 4
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101+gp_offset*3), module, ZeroCV2T::TRIG_SWITCH_4 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2+gp_offset*3), module, ZeroCV2T::TRIG_LED_4));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60+gp_offset*3), module, ZeroCV2T::CV_IN_4));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100+gp_offset*3), module, ZeroCV2T::TRIG_OUT_4));

	}
};


Model *modelZeroCV2T = createModel<ZeroCV2T, ZeroCV2TWidget>("ZeroCV2T");
