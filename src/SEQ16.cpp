//***********************************************************************************************
//16-step sequencer module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Based on SEQ16 VCV Rack by Autodafe http://www.autodafe.net
//Based on code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//***********************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"

struct SEQ16 : Module {
	enum ParamIds {
		CLOCK_PARAM,
		RUN_PARAM,
		RESET_PARAM,
		STEPS_PARAM,
		TRIGGER_PARAM,
		PREV_STEP,
		NEXT_STEP,
		GATE_MODE_PARAM,
		ROW1_PARAM,
		ROW2_PARAM = ROW1_PARAM + 16,
		ROW3_PARAM = ROW2_PARAM + 16,
		GATE_PARAM = ROW3_PARAM + 16,
		NUM_PARAMS = GATE_PARAM + 16
	};
	enum InputIds {
		CLOCK_INPUT,
		EXT_CLOCK_INPUT,
		RESET_INPUT,
		STEPS_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATES_OUTPUT,
		ROW1_OUTPUT,
		ROW2_OUTPUT,
		ROW3_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS = GATE_OUTPUT + 16
	};
	enum LightIds {
		RUNNING_LIGHT,
		RESET_LIGHT,
		GATES_LIGHT,
		TRIGGER_LIGHT,
		ROW_LIGHTS,
		GATE_LIGHTS = ROW_LIGHTS + 3,
		NUM_LIGHTS = GATE_LIGHTS + 16
	};

	bool running = true;
	bool triggerActive = false;
	// for external clock
	dsp::SchmittTrigger clockTrigger;
	// For buttons
	dsp::SchmittTrigger runningTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger prevTrigger;
	dsp::SchmittTrigger nextTrigger;
	dsp::SchmittTrigger manualTrigger;
	dsp::SchmittTrigger gateTriggers[16];

	float phase = 0.0f;
	float blinkPhase = 0.0f;
	int index = 0;
	int stepIndex = index+1;
	int modeIndex = 0;
	bool nextStep = false;
	bool gateState[16] = {};
	float resetLight = 0.0f;
	float stepLights[16] = {};
	const float lightLambda = 0.075f;
	int numSteps = 0;

	int seq_gate_mode = 0;// 0:TRIGGER, 1: RETRIGGER, 2:CONTINUOUS

	dsp::PulseGenerator gatePulse;

	SEQ16() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SEQ16::CLOCK_PARAM, -2.0f, 6.0f, 1.0f, "Clock Tempo", "BPM", 2.f, 60.f);
		configParam(SEQ16::STEPS_PARAM, 1.0f, 16.0f, 16.0f, "Step Length");

		for (int i = 0; i < 16; i++) {		
			configParam(SEQ16::ROW1_PARAM + i, 0.0f, 10.0f, 0.0f, "Step Value", " V");
			configParam(SEQ16::ROW2_PARAM + i, 0.0f, 10.0f, 0.0f, "Step Value", " V");
			configParam(SEQ16::ROW3_PARAM + i, 0.0f, 10.0f, 0.0f, "Step Value", " V");
			configParam(SEQ16::GATE_PARAM + i, 0.0f, 1.0f, 0.0f, "Step Gate", " V");
		}


		//New in V2, config switches info without displaying values
		configSwitch(GATE_MODE_PARAM,0.0f, 2.0f, 0.0f, "Trigger Mode", {"Trigger", "Retrigger", "Continuous"});

		//New in V2, config temporary buttons info without displaying values
		configButton(RUN_PARAM, "Run");
		configButton(RESET_PARAM, "Reset");
		configButton(TRIGGER_PARAM, "Manual Trigger (Step edit)");
		configButton(PREV_STEP, "Previous Step");
		configButton(NEXT_STEP, "Next Step");
		//new V2, port labels
		//Inputs
		configInput(CLOCK_INPUT, "Clock Mod CV");
		configInput(EXT_CLOCK_INPUT, "External Clock");
		configInput(RESET_INPUT, "External reset");
		configInput(STEPS_INPUT, "Steps Mod CV");

		//Outputs
		for (int i = 0; i < 16; i++) {		
			configOutput(GATE_OUTPUT + i, string::f("Step %d gate", i + 1));
		}
		configOutput(ROW1_OUTPUT, "Row 1 CV");
		configOutput(ROW2_OUTPUT, "Row 2 CV");
		configOutput(ROW3_OUTPUT, "Row 3 CV");
		configOutput(GATES_OUTPUT, "Gates");	



	}

	void onReset() override {
		for (int i = 0; i < 16; i++) {
			gateState[i] = true;
		}
	}

	void onRandomize() override {
		for (int i = 0; i < 16; i++) {
			gateState[i] = (random::uniform() > 0.5);
		}
	}

	void process(const ProcessArgs &args) override {
		numSteps = roundf(clamp(params[STEPS_PARAM].getValue(), 1.0f, 16.0f)); 
		stepIndex = index+1;
		seq_gate_mode = params[GATE_MODE_PARAM].getValue();


/* 		if(inputs[STEPS_INPUT].isConnected()){
			//cv mod for steps length not working properly, fix it
			steps_ext_cv =! steps_ext_cv;
			float steps_sum_value = params[STEPS_PARAM].getValue();
			params[STEPS_PARAM].setValue(clamp(round(params[STEPS_PARAM].getValue() + inputs[STEPS_INPUT].getVoltage()), 1.0f, 16.0f));
		}
 */
		// Run
		if (runningTrigger.process(params[RUN_PARAM].getValue())) {
			running = !running;
		}
		lights[RUNNING_LIGHT].value = running ? 1.0f : 0.0f;

		nextStep = false;

		if (running) {
			if (inputs[EXT_CLOCK_INPUT].isConnected()) {
				// External clock
				if (clockTrigger.process(inputs[EXT_CLOCK_INPUT].getVoltage())) {
					phase = 0.0f;
					nextStep = true;
				}
			}
			else {
				// Internal clock
				float clockTime = powf(2.0, params[CLOCK_PARAM].getValue() + inputs[CLOCK_INPUT].getVoltage());
				phase += clockTime * args.sampleTime;
				if (phase >= 1.0f) {
					phase -= 1.0f;
					nextStep = true;
				}
			}
		}

		// Reset
		if (resetTrigger.process(params[RESET_PARAM].getValue() + inputs[RESET_INPUT].getVoltage())) {
			phase = 0.0f;
			index = 16;
			nextStep = true;
			resetLight = 1.0;
		}

		if (nextStep) {
			// Advance step
			//todo fix cv mod for seq steps length
			int numSteps = clamp(round(params[STEPS_PARAM].getValue() + inputs[STEPS_INPUT].getVoltage()), 1.0f, 16.0f);
			index += 1;
			if (index >= numSteps) {
				index = 0;
			}
			stepLights[index] = 1.0f;
			gatePulse.trigger(1e-3);
		}

		resetLight -= resetLight / lightLambda * args.sampleTime;

		bool pulse = gatePulse.process(1.0 * args.sampleTime);

		// Gate buttons
		for (int i = 0; i < 16; i++) {
			if (gateTriggers[i].process(params[GATE_PARAM + i].getValue())) {
				gateState[i] = !gateState[i];
			}
			bool gateOn = (running && i == index && gateState[i]);
			//if (gateMode == TRIGGER)
			if(seq_gate_mode==0)
				gateOn = gateOn && pulse;
			//else if (gateMode == RETRIGGER)
			if(seq_gate_mode==1)
				gateOn = gateOn && !pulse;

			outputs[GATE_OUTPUT + i].setVoltage(gateOn ? 10.0f : 0.0f);
			stepLights[i] -= stepLights[i] / lightLambda * args.sampleTime;
			lights[GATE_LIGHTS + i].value = gateState[i] ? 1.0f - stepLights[i] : stepLights[i];
		}

		// Rows
		float row1 = params[ROW1_PARAM + index].getValue();
		float row2 = params[ROW2_PARAM + index].getValue();
		float row3 = params[ROW3_PARAM + index].getValue();
		bool gatesOn = (running && gateState[index]);
		//if (gateMode == TRIGGER)
		if(seq_gate_mode==0)
			gatesOn = gatesOn && pulse;
		//else if (gateMode == RETRIGGER)
		if(seq_gate_mode==1)
			gatesOn = gatesOn && !pulse;

		// Outputs
			outputs[ROW1_OUTPUT].setVoltage(row1);
			outputs[ROW2_OUTPUT].setVoltage(row2);
			outputs[ROW3_OUTPUT].setVoltage(row3);
			lights[RESET_LIGHT].value = resetLight;
			lights[GATES_LIGHT].value = gatesOn ? 1.0f : 0.0f;
			lights[ROW_LIGHTS].value = row1;
			lights[ROW_LIGHTS + 1].value = row2;
			lights[ROW_LIGHTS + 2].value = row3;
		//mod to make the manual trigger work
		if (running) {
			outputs[GATES_OUTPUT].setVoltage(gatesOn ? 10.0f : 0.0f);
			lights[TRIGGER_LIGHT].value = 0.0f;
			//disable manual trigger
			triggerActive = false;
		}

		//Edit mode
		if(manualTrigger.process(params[TRIGGER_PARAM].getValue())){
			triggerActive = !triggerActive;
		}
		lights[TRIGGER_LIGHT].value = triggerActive ? 1.0f : 0.0f;
		// Manual trigger/manual step, only when the seq is not running
		if (triggerActive) {
			running=false;
			outputs[GATES_OUTPUT].setVoltage(10.0f);

			// Blink light at 1Hz
			float deltaTime = 5.0f * args.sampleTime;
			blinkPhase += deltaTime;
			if (blinkPhase >= 1.0f){
				blinkPhase -= 1.0f;
			}
			// step edit light indicator
			for (int i = 0; i < 16; i++) {
				if(i==index){
					lights[GATE_LIGHTS + i].value = (blinkPhase < 0.5f) ? 1.0f : 0.0f;
				}else{
					lights[GATES_LIGHT].value = gatesOn ? 1.0f : 0.0f;
				}
			}

		}else{
			outputs[GATES_OUTPUT].setVoltage(gatesOn ? 10.0f : 0.0f);
			lights[GATES_LIGHT].value = gatesOn ? 1.0f : 0.0f;
		}
		//Prev/next step buttons only work when seq is not running
		if(!running){
			if (prevTrigger.process(params[PREV_STEP].getValue())) {
				index -= 1;
				if(index<0){
				index = numSteps-1;	
				}
			}
			if (nextTrigger.process(params[NEXT_STEP].getValue())) {
				index += 1;
				if(index>numSteps-1){
				index = 0;	
				}
			}
		}
	}


	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// running
		json_object_set_new(rootJ, "running", json_boolean(running));

		// gates
		json_t *gatesJ = json_array();
		for (int i = 0; i < 16; i++) {
			json_t *gateJ = json_integer((int) gateState[i]);
			json_array_append_new(gatesJ, gateJ);
		}
		json_object_set_new(rootJ, "gates", gatesJ);
		// gateMode
		json_t *gateModeJ = json_integer((int) seq_gate_mode);
		json_object_set_new(rootJ, "seq_gate_mode", gateModeJ);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// running
		json_t *runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);
		// gates
		json_t *gatesJ = json_object_get(rootJ, "gates");
		if (gatesJ) {
			for (int i = 0; i < 16; i++) {
				json_t *gateJ = json_array_get(gatesJ, i);
				if (gateJ)
					gateState[i] = !!json_integer_value(gateJ);
			}
		}

		// gateMode
		json_t *gateModeJ = json_object_get(rootJ, "seq_gate_mode");
		if (gateModeJ)
			seq_gate_mode = json_integer_value(gateModeJ);
	}

};


struct StepsDisplayWidget : TransparentWidget {

	int *value = NULL;
	std::shared_ptr<Font> font;
	std::string fontPath = asset::plugin(pluginInstance, "res/Segment7Standard.ttf");


	void drawLayer(const DrawArgs& args, int layer) override {
		if (layer != 1){
			return;
		}
		if (!value) {
			return;
		}
		font = APP->window->loadFont(fontPath);
		// text 
		if (font) {
			nvgFontSize(args.vg, 22);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 2.5);

			char displayStr[3];

			sprintf(displayStr, "%2u", (unsigned) *value);

			Vec textPos = Vec(6, 23);

			NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, displayStr,  NULL);
		}

}

};

template <typename BASE>
struct MuteLight : BASE {
	MuteLight() {
	  this->box.size = mm2px(Vec(6.0, 6.0));
	}
};


struct SEQ16Widget : ModuleWidget { 
	//Menu *createContextMenu() override;

	SEQ16Widget(SEQ16 *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SEQ16.svg")));
	
		//LCD STEPS SCREEN
		StepsDisplayWidget *display = new StepsDisplayWidget();
		display->box.pos = Vec(341,60);
		display->box.size = Vec(40, 30);
		if (module) {
			display->value = &module->numSteps;
		}
		addChild(display);
		//LCD CURRENT STEP SCREEN
		StepsDisplayWidget *display2 = new StepsDisplayWidget();
		display2->box.pos = Vec(401,60);
		display2->box.size = Vec(40, 30);
		if (module) {
			display2->value = &module->stepIndex;
		}
		addChild(display2);

		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//
		static const float portX[16] = {20,60,100,140,180,220,260,300,340,380,420,460,500,540,580,620};
		static const float elements_offst = 8;
		static const float main_lds_y = 64.4;
		//CLOCK KNOB
		addParam(createParam<as_KnobBlack>(Vec(portX[1]-elements_offst, 56), module, SEQ16::CLOCK_PARAM));
		//RUN RESET SWITCHES & LEDS
/* 		addParam(createParam<LEDBezel>(Vec(portX[2], main_lds_y), module, SEQ16::RUN_PARAM ));
		addChild(createLight<MuteLight<RedLight>>(Vec(portX[2]+2.2, main_lds_y+2), module, SEQ16::RUNNING_LIGHT)); */
		addParam(createParam<LEDBezel>(Vec(portX[2], main_lds_y), module, SEQ16::RUN_PARAM));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(portX[2]+2.2, main_lds_y+2), module, SEQ16::RUNNING_LIGHT));
/* 		addParam(createParam<LEDBezel>(Vec(portX[3], main_lds_y), module, SEQ16::RESET_PARAM ));
		addChild(createLight<MuteLight<RedLight>>(Vec(portX[3]+2.2, main_lds_y+2), module, SEQ16::RESET_LIGHT)); */
		addParam(createParam<LEDBezel>(Vec(portX[3], main_lds_y), module, SEQ16::RESET_PARAM));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(portX[3]+2.2, main_lds_y+2), module, SEQ16::RESET_LIGHT));

		//STEP TRIGGER
/* 		addParam(createParam<LEDBezel>(Vec(portX[11], main_lds_y+35), module, SEQ16::TRIGGER_PARAM ));
		addChild(createLight<MuteLight<RedLight>>(Vec(portX[11]+2.2, main_lds_y+2+35), module, SEQ16::TRIGGER_LIGHT)); */
		addParam(createParam<LEDBezel>(Vec(portX[11], main_lds_y+35), module, SEQ16::TRIGGER_PARAM));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(portX[11]+2.2, main_lds_y+2+35), module, SEQ16::TRIGGER_LIGHT));

		addParam(createParam<TL1105>(Vec(portX[9]+20, main_lds_y+40), module, SEQ16::PREV_STEP));
		addParam(createParam<TL1105>(Vec(portX[10]+5, main_lds_y+40), module, SEQ16::NEXT_STEP));
		//GATE MODE SWITCH
		addParam(createParam<as_CKSSThree>(Vec(portX[6]+2, main_lds_y-4), module, SEQ16::GATE_MODE_PARAM));
		//STEPS KNOBS
		addParam(createParam<as_KnobBlackSnap>(Vec(portX[7]-elements_offst, 56), module, SEQ16::STEPS_PARAM));
		static const float main_inputs_offst = 1;
		static const float main_inputs_y = 98;
		//SEQ VC INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(portX[1]- main_inputs_offst, main_inputs_y), module, SEQ16::CLOCK_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(portX[2]-main_inputs_offst, main_inputs_y), module, SEQ16::EXT_CLOCK_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(portX[3]-main_inputs_offst, main_inputs_y), module, SEQ16::RESET_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(portX[7]-main_inputs_offst, main_inputs_y), module, SEQ16::STEPS_INPUT));
		//GATE/ROW LEDS
		addChild(createLight<MediumLight<RedLight>>(Vec(portX[12]+elements_offst, main_lds_y+6), module, SEQ16::GATES_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(Vec(portX[13]+elements_offst, main_lds_y+6), module, SEQ16::ROW_LIGHTS));
		addChild(createLight<MediumLight<RedLight>>(Vec(portX[14]+elements_offst, main_lds_y+6), module, SEQ16::ROW_LIGHTS + 1));
		addChild(createLight<MediumLight<RedLight>>(Vec(portX[15]+elements_offst, main_lds_y+6), module, SEQ16::ROW_LIGHTS + 2));
		//GATE/ROW OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(portX[12], 98), module, SEQ16::GATES_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(portX[13], 98), module, SEQ16::ROW1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(portX[14], 98), module, SEQ16::ROW2_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(portX[15], 98), module, SEQ16::ROW3_OUTPUT));

		for (int i = 0; i < 16; i++) {
			//ROW KNOBS
			addParam(createParam<as_KnobBlack>(Vec(portX[i]-elements_offst, 157), module, SEQ16::ROW1_PARAM + i));
			addParam(createParam<as_KnobBlack>(Vec(portX[i]-elements_offst, 198), module, SEQ16::ROW2_PARAM + i));
			addParam(createParam<as_KnobBlack>(Vec(portX[i]-elements_offst, 240), module, SEQ16::ROW3_PARAM + i));
			//GATE LEDS
			addParam(createParam<LEDButton>(Vec(portX[i]+1.5, 284), module, SEQ16::GATE_PARAM + i));
			addChild(createLight<MediumLight<RedLight>>(Vec(portX[i]+5.8, 287.9), module, SEQ16::GATE_LIGHTS + i));
			//GATE STEPS OUT
			addOutput(createOutput<as_PJ301MPortGold>(Vec(portX[i]-2, 310), module, SEQ16::GATE_OUTPUT + i));
		}
	}


};

Model *modelSEQ16 = createModel<SEQ16, SEQ16Widget>("SEQ16");