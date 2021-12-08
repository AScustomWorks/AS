//**************************************************************************************
//Volt rescale module module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"

struct ReScale: Module {
    enum ParamIds {
		CONVERT_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        INPUT_0,
        INPUT_1,
		INPUT_2,
		INPUT_3,
        NUM_INPUTS
    };
    enum OutputIds {
	    OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    int selection = 0;
    float rescaled_value = 0.0f;
        float input_value = 0.0f;

    float getNoteInVolts(float noteValue) {
        int octaveInVolts = int(floorf(noteValue));
        float voltMinusOct = noteValue - octaveInVolts;
        return voltMinusOct;
    }

    ReScale() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(ReScale::CONVERT_PARAM, 1.0f, 4.0f, 1.0f, "Convert Mode");

		//new V2, port labels
		//inputs
		configInput(INPUT_0, "-5V/5V CV");
		configInput(INPUT_1, "0V/5V CV");
		configInput(INPUT_2, "0V/10V CV");
		configInput(INPUT_3, "1V/OCTAVE CV");
		//Outputs
		configOutput(OUTPUT, "CV");


    }

    void process(const ProcessArgs &args) override {

        selection = params[CONVERT_PARAM].getValue();

        if(inputs[INPUT_0].isConnected()){

            input_value = clamp(inputs[INPUT_0].getVoltage(), -5.0f,5.0f);
            if(selection==1){
                rescaled_value = input_value;
            }else if(selection==2){
                rescaled_value = rescale(input_value, -5.0f, 5.0f, 0.0f, 5.0f);
            }else if(selection==3){
                rescaled_value = rescale(input_value, -5.0f, 5.0f, -10.0f, 10.0f);
            }else if(selection==4){
                rescaled_value = rescale(input_value, -5.0f, 5.0f, 0.0f, 10.0f);
            }

        }else if(inputs[INPUT_1].isConnected()){

            input_value = clamp(inputs[INPUT_1].getVoltage(), 0.0f, 5.0f);
            if(selection==1){
                rescaled_value = rescale(input_value, 0.0f, 5.0f, -5.0f, 5.0f);
            }else if(selection==2){
                rescaled_value = input_value;
            }else if(selection==3){
                rescaled_value = rescale(input_value, 0.0f, 5.0f, -10.0f, 10.0f);
            }else if(selection==4){
                rescaled_value = rescale(input_value, -5.0f, 5.0f, 0.0f, 10.0f);
            }

        }else if(inputs[INPUT_2].isConnected()){
            
            input_value = clamp(inputs[INPUT_2].getVoltage(), 0.0f, 10.0f);
            if(selection==1){
                rescaled_value = rescale(input_value, 0.0f, 10.0f, -5.0f, 5.0f);
            }else if(selection==2){
                rescaled_value = rescale(input_value, 0.0f, 10.0f, 0.0f, 5.0f);        
            }else if(selection==3){
                rescaled_value = rescale(input_value, 0.0f, 10.0f, -10.0f, 10.0f);
            }else if(selection==4){
                rescaled_value = input_value;
            }

        }else if(inputs[INPUT_3].isConnected()){
            
            input_value = inputs[INPUT_3].getVoltage();
            if(selection==1){
                rescaled_value = input_value;
            }else if(selection==2){
                rescaled_value = input_value;      
            }else if(selection==3){
                rescaled_value = input_value;
            }else if(selection==4){
                //take the input of a midi KB, get the voltage minus octave, convert it to 1V/KEY
                float ext_key = getNoteInVolts(input_value);
                rescaled_value = clamp( rescale( ext_key, 0.0f, 1.0f, 0.0f, 11.0f ), 0.0f, 10.0f );
            }

        }
        outputs[OUTPUT].setVoltage(rescaled_value);
        
    }
    
};


////////////////////////////////////
struct ReScaleWidget : ModuleWidget {

    ReScaleWidget(ReScale *module) {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ReScale.svg")));
    
        //SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
        addChild(createWidget<as_HexScrew>(Vec(0, 0)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        //PORTS
        addInput(createInput<as_PJ301MPort>(Vec(18, 65), module, ReScale::INPUT_0));
        addInput(createInput<as_PJ301MPort>(Vec(18, 105), module, ReScale::INPUT_1));
        addInput(createInput<as_PJ301MPort>(Vec(18, 145), module, ReScale::INPUT_2));
        addInput(createInput<as_PJ301MPort>(Vec(18, 185), module, ReScale::INPUT_3));

        addParam(createParam<as_KnobBlackSnap4>(Vec(12, 226), module, ReScale::CONVERT_PARAM));

        addOutput(createOutput<as_PJ301MPortGold>(Vec(18, 280), module, ReScale::OUTPUT));


    }
};


Model *modelReScale = createModel<ReScale, ReScaleWidget>("ReScale");