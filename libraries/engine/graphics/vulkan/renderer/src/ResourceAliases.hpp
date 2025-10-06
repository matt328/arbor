#pragma once

#include <string>

namespace arb {

namespace BufferAlias {
const std::string IndirectCommand = "IndirectCommand";
const std::string IndirectCommandCount = "IndirectCommandCount";
const std::string IndirectMetaData = "IndirectMetaData";
const std::string ObjectData = "ObjectData";
const std::string ObjectPositions = "ObjectPositions";
const std::string ObjectRotations = "ObjectRotations";
const std::string ObjectScales = "ObjectScales";
const std::string GeometryRegion = "GeometryRegion";
const std::string FrameData = "FrameData";
const std::string ResourceTable = "ResourceTable";
const std::string Materials = "Materials";
};

namespace GlobalBufferAlias {
const std::string Index = "Index";
const std::string Position = "Position";
const std::string Normal = "Normal";
const std::string TexCoord = "TexCoord";
const std::string Color = "Color";
const std::string Animation = "Animation";
};
}
