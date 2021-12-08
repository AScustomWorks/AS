//**************************************************************************************
//TriggersMKIII module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"


struct TriggersMKIII: Module {
    enum ParamIds {
        TRIGGER_SWITCH_1,
        TRIGGER_SWITCH_2,
        NUM_PARAMS
    };
    enum InputIds {
        CV_TRIG_INPUT_1_1,
        CV_TRIG_INPUT_1_2,
        CV_TRIG_INPUT_2_1,
        CV_TRIG_INPUT_2_2,
        NUM_INPUTS
    };
    enum OutputIds {
        TRIGGER_OUT1,
        TRIGGER_OUT2,
        NUM_OUTPUTS
    };
    enum LightIds {
        TRIGGER_LED_1,
        TRIGGER_LED_2,
        NUM_LIGHTS
    };

    dsp::SchmittTrigger btnTrigger1, btnTrigger2;
    dsp::SchmittTrigger extTrigger1_1, extTrigger1_2;
    dsp::SchmittTrigger extTrigger2_1, extTrigger2_2;

    dsp::PulseGenerator triggerPulse1;
    bool trg_pulse1 = false;

    dsp::PulseGenerator triggerPulse2;
    bool trg_pulse2 = false;


    const float lightLambda = 0.075f;
    float resetLight1 = 0.0f;
    float resetLight2 = 0.0f;


    TriggersMKIII() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configButton(TRIGGER_SWITCH_1, "Momentary Trigger 1");
        configButton(TRIGGER_SWITCH_2, "Momentary Trigger 2");

		configInput(CV_TRIG_INPUT_1_1, "CV external trigger 1");
		configInput(CV_TRIG_INPUT_1_2, "CV external trigger 1");
        configInput(CV_TRIG_INPUT_2_1, "CV external trigger 2");
		configInput(CV_TRIG_INPUT_2_2, "CV external trigger 2");
		//Outputs
		configOutput(TRIGGER_OUT1, "Trigger 1");
		configOutput(TRIGGER_OUT2, "Trigger 2");


    }


	std::string label1, label2;

    bool dirty1 = false;
    bool dirty2 = false;

	void onReset() override {
		label1 = "";
        label2 = "";
        dirty1 = true;
        dirty2 = true;
	}

    //json updates for V2
	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "label1", json_stringn(label1.c_str(), label1.size()));
        json_object_set_new(rootJ, "label2", json_stringn(label2.c_str(), label2.size()));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* text1J = json_object_get(rootJ, "label1");
		if (text1J){
			label1 = json_string_value(text1J);
            dirty1 = true;
        }
        json_t* text2J = json_object_get(rootJ, "label2");
        if (text2J){
			label2 = json_string_value(text2J);
            dirty2 = true;
        }
	}


    void process(const ProcessArgs &args) override {

        //TRIGGER 1
        if (btnTrigger1.process(params[TRIGGER_SWITCH_1].getValue()) || extTrigger1_1.process(inputs[CV_TRIG_INPUT_1_1].getVoltage()) || extTrigger1_2.process(inputs[CV_TRIG_INPUT_1_2].getVoltage())) {
            resetLight1 = 1.0;
            triggerPulse1.trigger(1e-3f);
        }
        trg_pulse1 = triggerPulse1.process(1.0 * args.sampleTime);
        outputs[TRIGGER_OUT1].setVoltage((trg_pulse1 ? 10.0f : 0.0f));

        resetLight1 -= resetLight1 / lightLambda * args.sampleTime;
        lights[TRIGGER_LED_1].value = resetLight1;

        //TRIGGER 2
        if (btnTrigger2.process(params[TRIGGER_SWITCH_2].getValue()) || extTrigger2_1.process(inputs[CV_TRIG_INPUT_2_1].getVoltage()) || extTrigger2_2.process(inputs[CV_TRIG_INPUT_2_2].getVoltage())) {
            resetLight2 = 1.0;
            triggerPulse2.trigger(1e-3f);
        }

        trg_pulse2 = triggerPulse2.process(1.0 * args.sampleTime);
        outputs[TRIGGER_OUT2].setVoltage((trg_pulse2 ? 10.0f : 0.0f));

        resetLight2 -= resetLight2 / lightLambda * args.sampleTime;
        lights[TRIGGER_LED_2].value = resetLight2;
        
    }
    

};
//Displays code updated for V2
//Display 1
struct TriggersTextField1 : LedDisplayTextField {
	TriggersMKIII* module;

	void step() override {
		LedDisplayTextField::step();
		if (module && module->dirty1) {
			setText(module->label1);
			module->dirty1 = false;
		}
	}

	void onChange(const ChangeEvent& e) override {
		if (module)
			module->label1 = getText();
	}
};


struct TriggersDisplay1 : LedDisplay {
	void setModule(TriggersMKIII* module) {
		TriggersTextField1* textField1 = createWidget<TriggersTextField1>(Vec(0, 0));
		textField1->box.size = box.size;
		textField1->multiline = false;
		textField1->module = module;
		addChild(textField1);
	}
};

//Display 2

struct TriggersTextField2 : LedDisplayTextField {
	TriggersMKIII* module;

	void step() override {
		LedDisplayTextField::step();
		if (module && module->dirty2) {
			setText(module->label2);
			module->dirty2 = false;
		}
	}

	void onChange(const ChangeEvent& e) override {
		if (module)
			module->label2 = getText();
	}
};


struct TriggersDisplay2 : LedDisplay {
	void setModule(TriggersMKIII* module) {
		TriggersTextField2* textField2 = createWidget<TriggersTextField2>(Vec(0, 0));
		textField2->box.size = box.size;
		textField2->multiline = false;
		textField2->module = module;
		addChild(textField2);
	}
};
//end displays code

struct TriggersMKIIIWidget : ModuleWidget { 
    
    
    TriggersMKIIIWidget(TriggersMKIII *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TriggersMKIII.svg")));
    
        //SCREWS
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        static const float led_offset = 6.0;//3.3;
        static const float led_center = 15;
        static const float y_offset = 150;
        //TRIGGER 1

		TriggersDisplay1* display1 = createWidget<TriggersDisplay1>(Vec(6, 46));
		display1->box.size = Vec(78, 30);
		display1->setModule(module);
		addChild(display1);
        
        //SWITCH

        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 122), module, TriggersMKIII::TRIGGER_SWITCH_1));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 122+led_offset), module, TriggersMKIII::TRIGGER_LED_1));

        //PORTS
        addInput(createInput<as_PJ301MPort>(Vec(8, 90), module, TriggersMKIII::CV_TRIG_INPUT_1_1));
        addInput(createInput<as_PJ301MPort>(Vec(33, 90), module, TriggersMKIII::CV_TRIG_INPUT_1_2));
        addOutput(createOutput<as_PJ301MPortGold>(Vec(58, 90), module, TriggersMKIII::TRIGGER_OUT1));

        //TRIGGER 2

		TriggersDisplay2* display2 = createWidget<TriggersDisplay2>(Vec(6, 46+y_offset));
		display2->box.size = Vec(78, 30);
		display2->setModule(module);
		addChild(display2);
        
        //SWITCH

        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 122+y_offset), module, TriggersMKIII::TRIGGER_SWITCH_2));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 122+led_offset+y_offset), module, TriggersMKIII::TRIGGER_LED_2));

        //PORTS
        addInput(createInput<as_PJ301MPort>(Vec(8, 90+y_offset), module, TriggersMKIII::CV_TRIG_INPUT_2_1));
        addInput(createInput<as_PJ301MPort>(Vec(33, 90+y_offset), module, TriggersMKIII::CV_TRIG_INPUT_2_2));
        addOutput(createOutput<as_PJ301MPortGold>(Vec(58, 90+y_offset), module, TriggersMKIII::TRIGGER_OUT2));
    
    }   


};


Model *modelTriggersMKIII = createModel<TriggersMKIII, TriggersMKIIIWidget>("TriggersMKIII");
