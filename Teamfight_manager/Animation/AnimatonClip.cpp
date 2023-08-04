#include "stdafx.h"
#include "rapidcsv.h"

bool AnimationClip::LoadFromFile(const std::string path)
{
    rapidcsv::Document doc(path, rapidcsv::LabelParams(-1,-1));
    id = doc.GetCell<std::string>(0, 1);
    fps = doc.GetCell<int>(1, 1);
    loopType = (AnimationLoopTypes)doc.GetCell<int>(2, 1);

    for (int i = 4; i < doc.GetRowCount(); ++i)
    {
        auto rows = doc.GetRow<std::string>(i);
        frames.push_back({
            rows[0] ,
            { std::stoi(rows[1]), std::stoi(rows[2]),
            std::stoi(rows[3]), std::stoi(rows[4]) }
            });
    }
    return true;
}
