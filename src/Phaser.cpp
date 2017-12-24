//**************************************************************************************
//Phaser VCV Rack mods by Alfredo Santamaria  - AS -
//
//Based on the Phaser Module for VCV Rack by Autodafe http://www.autodafe.net
//Based on code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//And part of code on musicdsp.org: http://musicdsp.org/showArchiveComment.php?ArchiveID=78
//**************************************************************************************

#include "AS.hpp"
#include "dsp/digital.hpp"

#include <stdlib.h>

struct PhaserFx : Module{
	enum ParamIds {
		PARAM_RATE,
		PARAM_FEEDBACK,
		PARAM_DEPTH,
        BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT,
		NUM_OUTPUTS
	};
	  enum LightIds {
		BYPASS_LED,
		NUM_LIGHTS
	};

	SchmittTrigger bypass_button_trig;

	bool fx_bypass = false;
	PhaserFx() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	void step() override;

	json_t *toJson()override {
		json_t *rootJm = json_object();

		json_t *statesJ = json_array();
		
			json_t *bypassJ = json_boolean(fx_bypass);
			json_array_append_new(statesJ, bypassJ);
		
		json_object_set_new(rootJm, "as_FxBypass", statesJ);

		return rootJm;
	}

	void fromJson(json_t *rootJm)override {
		json_t *statesJ = json_object_get(rootJm, "as_FxBypass");
		
			json_t *bypassJ = json_array_get(statesJ, 0);

			fx_bypass = !!json_boolean_value(bypassJ);
		
	}

};

#define SR (44100.f)  //sample rate
#define F_PI (3.14159f)

class Phaser{
public:
	Phaser()  //initialise to some usefull defaults...
    	: _fb( .7f )
    	, _lfoPhase( 0.f )
    	, _depth( 1.f )
        , _zm1( 0.f )
    {
    	Range( 440.f, 1600.f );
    	Rate( .5f );
    }

    void Range( float fMin, float fMax ){ // Hz
    	_dmin = fMin / (SR/2.f);
        _dmax = fMax / (SR/2.f);
    }

    void Rate( float rate ){ // cps
    	_lfoInc = 2.f * F_PI * (rate / SR);
    }

    void Feedback( float fb ){ // 0 -> <1.
    	_fb = fb;
    }

    void Depth( float depth ){  // 0 -> 1.
     	_depth = depth;
    }

    float Update( float inSamp ){
    	//calculate and update phaser sweep lfo...
        float d  = _dmin + (_dmax-_dmin) * ((sin( _lfoPhase ) + 
1.f)/2.f);
        _lfoPhase += _lfoInc;
        if( _lfoPhase >= F_PI * 2.f )
        	_lfoPhase -= F_PI * 2.f;

        //update filter coeffs
        for( int i=0; i<6; i++ )
        	_alps[i].Delay( d );

        //calculate output
        float y = 	_alps[0].Update(
        			 _alps[1].Update(
                      _alps[2].Update(
                       _alps[3].Update(
                        _alps[4].Update(
                         _alps[5].Update( inSamp + _zm1 * _fb ))))));
        _zm1 = y;

        return inSamp + y * _depth;
    }
private:
	class AllpassDelay{
    public:
    	AllpassDelay()
        	: _a1( 0.f )
            , _zm1( 0.f )
            {}

        void Delay( float delay ){ //sample delay time
        	_a1 = (1.f - delay) / (1.f + delay);
        }

        float Update( float inSamp ){
        	float y = inSamp * -_a1 + _zm1;
        	_zm1 = y * _a1 + inSamp;

            return y;
        }
    private:
    	float _a1, _zm1;
    };

    AllpassDelay _alps[6];

    float _dmin, _dmax; //range
    float _fb; //feedback
    float _lfoPhase;
    float _lfoInc;
    float _depth;

    float _zm1;
};

Phaser *pha = new Phaser();

void PhaserFx::step() {

  if (bypass_button_trig.process(params[BYPASS_SWITCH].value))
    {
		  fx_bypass = !fx_bypass;
	  }
    lights[BYPASS_LED].value = fx_bypass ? 1.0 : 0.0;


	float rate = params[PARAM_RATE].value;
	float feedback = params[PARAM_FEEDBACK].value;
	float depth = params[PARAM_DEPTH].value;

	float input = inputs[INPUT].value / 5.0;

		pha->Rate(rate);
		pha->Feedback(feedback);
		pha->Depth (depth);
	
	float out = pha->Update(input);

	//check bypass switch status
	if (fx_bypass)
    {
		 outputs[OUT].value = input * 5;
	  }else
		outputs[OUT].value= out * 5;

}

PhaserFxWidget::PhaserFxWidget() {
	PhaserFx *module = new PhaserFx();
	setModule(module);
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;	
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Phaser.svg")));
		addChild(panel); 
	}

	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    //KNOBS  
	addParam(createParam<as_KnobBlack>(Vec(25, 60), module, PhaserFx::PARAM_RATE, 0, 1, 0));
	addParam(createParam<as_KnobBlack>(Vec(25, 125), module, PhaserFx::PARAM_FEEDBACK, 0, 0.95, 0));
	addParam(createParam<as_KnobBlack>(Vec(25, 190), module, PhaserFx::PARAM_DEPTH, 0, 1, 0));
    //BYPASS SWITCH
  	addParam(createParam<LEDBezel>(Vec(33, 260), module, PhaserFx::BYPASS_SWITCH , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(35.2, 262), module, PhaserFx::BYPASS_LED));
    //INS/OUTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 310), module, PhaserFx::INPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 310), module, PhaserFx::OUT));
 
}
