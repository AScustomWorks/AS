//**************************************************************************************
//CV to Trigger convenrter module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"

struct Cv2T : Module {
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

	dsp::PulseGenerator trigPulse1, trigPulse2, trigPulse3, trigPulse4;
	bool trig_pulse_1 = false;
	bool trig_pulse_2 = false;
	bool trig_pulse_3 = false;
	bool trig_pulse_4 = false;

	float trigger_length = 0.0001f;

	const float lightLambda = 0.075f;
	float trigLight1 = 0.0f;
	float trigLight2 = 0.0f;
	float trigLight3 = 0.0f;
	float trigLight4 = 0.0f;

	bool cv_1_engaged = false;
	bool cv_2_engaged = false;
	bool cv_3_engaged = false;
	bool cv_4_engaged = false;

	float current_cv_1_volts = 0.0f;
	float current_cv_2_volts = 0.0f;
	float current_cv_3_volts = 0.0f;
	float current_cv_4_volts = 0.0f;

	Cv2T() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
/* 		configParam(Cv2T::TRIG_SWITCH_1 , 0.0f, 1.0f, 0.0f, "CH 1 Trigger");
		configParam(Cv2T::TRIG_SWITCH_2 , 0.0f, 1.0f, 0.0f, "CH 2 Trigger");
		configParam(Cv2T::TRIG_SWITCH_3 , 0.0f, 1.0f, 0.0f, "CH 3 Trigger");
		configParam(Cv2T::TRIG_SWITCH_4 , 0.0f, 1.0f, 0.0f, "CH 4 Trigger"); */

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
		//CV 2 TRIG 1
		if ( trig_1.process( params[TRIG_SWITCH_1].getValue() ) ) {
			trigLight1 = 1.0;
			trigPulse1.trigger( trigger_length );
		}
		current_cv_1_volts = inputs[CV_IN_1].getVoltage();

		if ( !cv_1_engaged ) {
			if ( current_cv_1_volts > 0.0f ) {
				cv_1_engaged = true;
				trigLight1 = 1.0;
				trigPulse1.trigger( trigger_length );
				// send start trigger
			}
		} else {
			if ( (int)current_cv_1_volts <= 0 ) {
				// send stop trigger
				trigLight1 = 1.0;
				trigPulse1.trigger( trigger_length );
				cv_1_engaged = false;
			}
		}

		trigLight1 -= trigLight1 / lightLambda / args.sampleRate;
		lights[TRIG_LED_1].value = trigLight1;
		trig_pulse_1 = trigPulse1.process( 1.0 / args.sampleRate );
		outputs[TRIG_OUT_1].setVoltage(( trig_pulse_1 ? 10.0f : 0.0f ));

		//CV 2 TRIG 2
		if ( trig_2.process( params[TRIG_SWITCH_2].getValue() ) ) {
			trigLight2 = 1.0;
			trigPulse2.trigger( trigger_length );
		}
		current_cv_2_volts = inputs[CV_IN_2].getVoltage();

		if ( !cv_2_engaged ) {
			if ( current_cv_2_volts > 0.0f ) {
				cv_2_engaged = true;
				trigLight2 = 1.0;
				trigPulse2.trigger( trigger_length );
				// send start trigger
			}
		} else {
			if ( (int)current_cv_2_volts <= 0 ) {
				// send stop trigger
				trigLight2 = 1.0;
				trigPulse2.trigger( trigger_length );
				cv_2_engaged = false;
			}
		}

		trigLight2 -= trigLight2 / lightLambda / args.sampleRate;
		lights[TRIG_LED_2].value = trigLight2;
		trig_pulse_2 = trigPulse2.process( 1.0 / args.sampleRate );
		outputs[TRIG_OUT_2].setVoltage(( trig_pulse_2 ? 10.0f : 0.0f ));

		//CV 2 TRIG 3
		if ( trig_3.process( params[TRIG_SWITCH_3].getValue() ) ) {
			trigLight3 = 1.0;
			trigPulse3.trigger( trigger_length );
		}
		current_cv_3_volts = inputs[CV_IN_3].getVoltage();

		if ( !cv_3_engaged ) {
			if ( current_cv_3_volts > 0.0f ) {
				cv_3_engaged = true;
				trigLight3 = 1.0;
				trigPulse3.trigger( trigger_length );
				// send start trigger
			}
		} else {
			if ( (int)current_cv_3_volts <= 0 ) {
				// send stop trigger
				trigLight3 = 1.0;
				trigPulse3.trigger( trigger_length );
				cv_3_engaged = false;
			}
		}

		trigLight3 -= trigLight3 / lightLambda / args.sampleRate;
		lights[TRIG_LED_3].value = trigLight3;
		trig_pulse_3 = trigPulse3.process( 1.0 / args.sampleRate );
		outputs[TRIG_OUT_3].setVoltage(( trig_pulse_3 ? 10.0f : 0.0f ));

		//CV 2 TRIG 4
		if ( trig_4.process( params[TRIG_SWITCH_4].getValue() ) ) {
			trigLight4 = 1.0;
			trigPulse4.trigger( trigger_length );
		}
		current_cv_4_volts = inputs[CV_IN_4].getVoltage();

		if ( !cv_4_engaged ) {
			if ( current_cv_4_volts > 0.0f ) {
				cv_4_engaged = true;
				trigLight4 = 1.0;
				trigPulse4.trigger( trigger_length );
				// send start trigger
			}
		} else {
			if ( (int)current_cv_4_volts <= 0 ) {
				// send stop trigger
				trigLight4 = 1.0;
				trigPulse4.trigger( trigger_length );
				cv_4_engaged = false;
			}
		}

		trigLight4 -= trigLight4 / lightLambda / args.sampleRate;
		lights[TRIG_LED_4].value = trigLight4;
		trig_pulse_4 = trigPulse4.process( 1.0 / args.sampleRate );
		outputs[TRIG_OUT_4].setVoltage(( trig_pulse_4 ? 10.0f : 0.0f ));

	}
};


struct Cv2TWidget : ModuleWidget { 

	Cv2TWidget(Cv2T *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CV2T.svg")));
	
		//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
		addChild(createWidget<as_HexScrew>(Vec(0, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const int gp_offset = 75;
		//CV 2 TRIG 1
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101), module, Cv2T::TRIG_SWITCH_1 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2), module, Cv2T::TRIG_LED_1));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60), module, Cv2T::CV_IN_1));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100), module, Cv2T::TRIG_OUT_1));
		//CV 2 TRIG 2
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101+gp_offset*1), module, Cv2T::TRIG_SWITCH_2 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2+gp_offset*1), module, Cv2T::TRIG_LED_2));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60+gp_offset*1), module, Cv2T::CV_IN_2));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100+gp_offset*1), module, Cv2T::TRIG_OUT_2));
		//CV 2 TRIG 3
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101+gp_offset*2), module, Cv2T::TRIG_SWITCH_3 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2+gp_offset*2), module, Cv2T::TRIG_LED_3));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60+gp_offset*2), module, Cv2T::CV_IN_3));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100+gp_offset*2), module, Cv2T::TRIG_OUT_3));
		//CV 2 TRIG 4
		//SWITCH & LED
		addParam(createParam<LEDBezel>(Vec(6, 101+gp_offset*3), module, Cv2T::TRIG_SWITCH_4 ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(6+2.2, 103.2+gp_offset*3), module, Cv2T::TRIG_LED_4));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18,60+gp_offset*3), module, Cv2T::CV_IN_4));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(32, 100+gp_offset*3), module, Cv2T::TRIG_OUT_4));
		
	}
};


Model *modelCv2T = createModel<Cv2T, Cv2TWidget>("Cv2T");