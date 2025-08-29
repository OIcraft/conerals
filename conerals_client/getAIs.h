// get AIs
#include "humanOptions.h"
#include ".\AI\example1.h"
#include ".\AI\example2.h"
#include ".\AI\PhirainEX.h"
#include ".\AI\OI_minecraft.h"
#include ".\AI\feiwu.h"
#include ".\AI\PhiFFA.h"
#include ".\AI\PhiCore.h"
#include ".\AI\Explorer.h"
#include ".\AI\DeepSeek.h"
#include ".\AI\laser.h"
#include ".\AI\aaa1145141919810.h"
// initialize
void Initialize(string currentAIname){
    if (currentAIname == "human"){
        humanOptions::Init();
        return;
    }
    else if (currentAIname == "example1"){
        example1::Init();
        return;
    }
    else if (currentAIname == "example2"){
        example2::Init();
        return;
    }
    else if (currentAIname == "PhirainEX"){
        PhirainEX::Init();
        return;
    }
    else if (currentAIname == "OI_minecraft"){
        OI_minecraft::Init();
        return;
    }
    else if (currentAIname == "feiwu"){
        feiwu::Init();
        return;
    }
    else if (currentAIname == "PhiFFA"){
        PhiFFA::Init();
        return;
    }
    else if (currentAIname == "PhiCore"){
        PhiCore::Init();
        return;
    }
    else if (currentAIname == "Explorer"){
        Explorer::Init();
        return;
    }
    else if (currentAIname == "DeepSeek"){
        DeepSeek::Init();
        return;
    }
    else if (currentAIname == "laser"){
        laser::Init();
        return;
    }
    else if (currentAIname == "aaa1145141919810"){
        aaa1145141919810::Init();
        return;
    }
    return;
}
// get moves
movement GetMove(string currentAIname){
    if (currentAIname == "human"){
        humanOptions::GetTheTime(theStartTimeOfTheCurrentTurn);
        return humanOptions::Move();
    }
    else if (currentAIname == "example1"){
        return example1::Move();
    }
    else if (currentAIname == "example2"){
        return example2::Move();
    }
    else if (currentAIname == "PhirainEX"){
        return PhirainEX::Move();
    }
    else if (currentAIname == "OI_minecraft"){
        return OI_minecraft::Move();
    }
    else if (currentAIname == "feiwu"){
        return feiwu::Move();
    }
    else if (currentAIname == "PhiFFA"){
        return PhiFFA::Move();
    }
    else if (currentAIname == "PhiCore"){
        return PhiCore::Move();
    }
    else if (currentAIname == "Explorer"){
        return Explorer::Move();
    }
    else if (currentAIname == "DeepSeek"){
        return DeepSeek::Move();
    }
    else if (currentAIname == "laser"){
        return laser::Move();
    }
    else if (currentAIname == "aaa1145141919810"){
        return aaa1145141919810::Move();
    }
    return (movement){-1, -1, 0};
}
