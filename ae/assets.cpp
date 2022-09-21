/******************************************************************************
* Copyright (c) 2022 Alan Witkowski
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*******************************************************************************/
#include <ae/assets.h>
#include <ae/ui.h>
#include <ae/program.h>
#include <ae/font.h>
#include <ae/texture.h>
#include <ae/texture_array.h>
#include <ae/tilemap.h>
#include <ae/atlas.h>
#include <ae/mesh.h>
#include <ae/animation.h>
#include <ae/files.h>
#include <ae/graphics.h>
#include <ae/audio.h>
#include <constants.h>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <limits>
#include <tinyxml2/tinyxml2.h>

namespace ae {

_Assets Assets;

// Initialize
void _Assets::Init() {
}

// Shutdown
void _Assets::Close() {

	for(const auto &Program : Programs)
		delete Program.second;

	for(const auto &Shader : Shaders)
		delete Shader.second;

	for(const auto &Texture : Textures)
		delete Texture.second;

	for(const auto &TextureArray : TextureArrays)
		delete TextureArray.second;

	for(const auto &TileMap : TileMaps)
		delete TileMap.second;

	for(const auto &Atlas : Atlases)
		delete Atlas.second;

	for(const auto &Mesh : Meshes)
		delete Mesh.second;

	for(const auto &Font : Fonts)
		delete Font.second;

	for(const auto &Sound : Sounds)
		delete Sound.second;

	for(const auto &Song : Music)
		delete Song.second;

	for(const auto &Style : Styles)
		delete Style.second;

	for(const auto &Reel : Reels)
		delete Reel.second;

	Fonts.clear();
	Layers.clear();
	Textures.clear();
	TextureArrays.clear();
	TileMaps.clear();
	Atlases.clear();
	Meshes.clear();
	Styles.clear();
	Reels.clear();
	Animations.clear();
	Sounds.clear();
	Music.clear();
	Elements.clear();
}

// Loads the fonts
void _Assets::LoadFonts(const std::string &Path, bool LoadFonts) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read the file
	while(!File.eof() && File.peek() != EOF) {

		// Read strings
		std::string Name;
		std::string FontFile;
		std::string ProgramName;
		std::getline(File, Name, '\t');
		std::getline(File, FontFile, '\t');
		std::getline(File, ProgramName, '\t');

		// Check for duplicates
		if(!LoadFonts && Fonts[Name])
			throw std::runtime_error(std::string(__func__) + " duplicate id '" + Name + "'");

		// Find program
		if(Programs.find(ProgramName) == Programs.end())
			throw std::runtime_error(std::string(__func__) + " unknown program '" + ProgramName + "'");

		// Get attributes
		uint32_t Size;
		int Scaled;
		File >> Size >> Scaled;

		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Load font
		if(LoadFonts) {

			// Check for font name
			if(Fonts.find(Name) == Fonts.end())
				throw std::runtime_error(std::string(__func__) + " unknown font '" + Name + "'");

			// Load font
			float ScaleFactor = Scaled ? _Element::GetUIScale() : 1.0f;
			Fonts[Name]->Load(Name, FontFile, Programs[ProgramName], Size * ScaleFactor);
		}
		else {

			// Create empty font
			_Font *Font = new _Font();
			Font->ID = Name;
			Fonts[Name] = Font;
		}
	}

	File.close();
}

// Load render layers
void _Assets::LoadLayers(const std::string &Path) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read the file
	while(!File.eof() && File.peek() != EOF) {
		std::string Name;
		std::getline(File, Name, '\t');

		// Get layer
		_Layer Layer;
		File >> Layer.Layer >> Layer.DepthTest >> Layer.DepthMask >> Layer.EditorOnly;

		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Set layer
		Layers[Name] = Layer;
	}

	File.close();
}

// Load shader programs
void _Assets::LoadPrograms(const std::string &Path) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read the file
	while(!File.eof() && File.peek() != EOF) {
		std::string Name;
		std::string VertexPath;
		std::string FragmentPath;
		std::getline(File, Name, '\t');
		std::getline(File, VertexPath, '\t');
		std::getline(File, FragmentPath, '\t');

		// Get integer parameters
		GLuint Attribs;
		size_t MaxLights;
		File >> Attribs >> MaxLights;

		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Check for duplicates
		if(Programs[Name])
			throw std::runtime_error(std::string(__func__) + " duplicate id '" + Name + "'");

		// Load vertex shader
		if(Shaders.find(VertexPath) == Shaders.end())
			Shaders[VertexPath] = new _Shader(VertexPath, GL_VERTEX_SHADER);

		// Load fragment shader
		if(Shaders.find(FragmentPath) == Shaders.end())
			Shaders[FragmentPath] = new _Shader(FragmentPath, GL_FRAGMENT_SHADER);

		// Create program
		Programs[Name] = new _Program(Name, Shaders[VertexPath], Shaders[FragmentPath], Attribs, MaxLights);
	}

	File.close();
}

// Loads the color table
void _Assets::LoadColors(const std::string &Path) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Add default color
	glm::vec4 Color(1.0f);
	Colors[""] = Color;

	// Read table
	while(!File.eof() && File.peek() != EOF) {

		std::string Name;
		std::getline(File, Name, '\t');

		File >> Color.r >> Color.g >> Color.b >> Color.a;
		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Check for duplicates
		if(Colors.find(Name) != Colors.end())
			throw std::runtime_error(std::string(__func__) + " duplicate id '" + Name + "'");

		Colors[Name] = Color;
	}

	File.close();
}

// Load a directory full of textures
void _Assets::LoadTextureDirectory(const std::string &Path, const _TextureSettings &TextureSettings) {

	// Get files
	_Files Files(Path);

	// Load textures
	for(const auto &File : Files.Nodes) {
		std::string Name = Path + File;
		if(!Textures[Name])
			Textures[Name] = new _Texture(Name, TextureSettings);
	}
}

// Load texture pack
void _Assets::LoadTexturePack(const std::string &Path, const _TextureSettings &TextureSetting) {

	// Load file pack
	_FilePack FilePack(Path);

	// Load textures
	FILE *FileHandle = fopen(Path.c_str(), "rb");
	if(!FileHandle)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	for(const auto &File : FilePack.Data) {
		if(Textures[File.first])
			continue;

		fseek(FileHandle, File.second.Offset + FilePack.BodyOffset, SEEK_SET);
		Textures[File.first] = new _Texture(File.first, FileHandle, TextureSetting);
	}

	fclose(FileHandle);
}

// Load atlases
void _Assets::LoadAtlasDirectory(const std::string &Path, const glm::ivec2 &Size, float Padding) {

	// Get files
	_Files Files(Path);

	// Create atlases
	for(const auto &File : Files.Nodes) {
		std::string Name = Path + File;
		if(!Atlases[Name])
			Atlases[Name] = new _Atlas(Textures[Name], Size, Padding);
	}
}

// Load game audio
void _Assets::LoadSounds(const std::string &Path) {

	// Get files
	_Files Files(Path);

	// Load audio
	for(const auto &File : Files.Nodes) {
		if(!Sounds[File])
			Sounds[File] = Audio.LoadSound(Path + File);
	}
}

// Load sound pack
void _Assets::LoadSoundPack(const std::string &Path) {

	// Load file pack
	_FilePack FilePack(Path);

	// Load sounds
	FILE *FileHandle = fopen(Path.c_str(), "rb");
	if(!FileHandle)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	for(const auto &File : FilePack.Data) {
		if(Sounds.find(File.second.Name) != Sounds.end())
			continue;

		int Start = File.second.Offset + FilePack.BodyOffset;
		fseek(FileHandle, Start, SEEK_SET);
		Sounds[File.second.Name] = Audio.LoadSound(_AudioFile(FileHandle, Start, File.second.Size));
	}

	// Close
	fclose(FileHandle);
}

// Load music files
void _Assets::LoadMusic(const std::string &Path) {

	// Get files
	_Files Files(Path);

	// Load audio
	for(const auto &File : Files.Nodes) {
		if(!Music[File])
			Music[File] = Audio.LoadMusic(Path + File);
	}
}

// Load meshes
void _Assets::LoadMeshDirectory(const std::string &Path) {

	// Get files
	_Files Files(Path);

	// Load meshes
	for(const auto &File : Files.Nodes) {
		if(File.find(".mesh") != std::string::npos) {
			std::string Name = Path + File;
			Meshes[Name] = new _Mesh(Name);
		}
	}
}

// Load animation reels
void _Assets::LoadReels(const std::string &Path, bool SetNameOnly) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read file
	while(!File.eof() && File.peek() != EOF) {
		std::string Name;
		std::getline(File, Name, '\t');

		// Check for duplicates
		if(Reels[Name])
			throw std::runtime_error(std::string(__func__) + " duplicate id '" + Name + "'");

		// Create template
		_Reel *Template = new _Reel();
		Template->ID = Name;

		// Load texture
		std::string TextureFile;
		std::getline(File, TextureFile, '\t');
		if(!Textures[TextureFile])
			throw std::runtime_error(std::string(__func__) + " unknown texture '" + TextureFile + "'");

		Template->Texture = Textures[TextureFile];

		// Read data
		File >> Template->FramePeriod >> Template->FrameSize.x >> Template->FrameSize.y >> Template->StartFrame >> Template->EndFrame >> Template->DefaultFrame >> Template->RepeatType;
		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Add to list
		if(!SetNameOnly) {
			Template->FramesPerLine = Template->Texture->Size.x / Template->FrameSize.x;
			Template->TextureScale = glm::vec2(Template->FrameSize) / glm::vec2(Template->Texture->Size);
		}
		Reels[Name] = Template;
	}

	File.close();
}

// Load animations
void _Assets::LoadAnimations(const std::string &Path) {

	// Load file
	std::ifstream File(Path, std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read file
	while(!File.eof() && File.peek() != EOF) {
		std::string ID;
		std::getline(File, ID, '\t');

		// Check for duplicates
		if(Animations.find(ID) != Animations.end())
			throw std::runtime_error(std::string(__func__) + " duplicate id '" + ID + "'");

		// Read rest of line into buffer
		std::string Line;
		std::getline(File, Line, '\n');
		std::stringstream Buffer(Line);

		// Get reels
		std::string ReelID;
		std::vector<const _Reel *> AnimationReels;
		while(std::getline(Buffer, ReelID, '\t')) {
			if(ReelID != "" && Reels.find(ReelID) == Reels.end())
				throw std::runtime_error(std::string(__func__) + " unknown reel_id '" + ReelID + "'");

			AnimationReels.push_back(Reels[ReelID]);
		}

		Animations[ID] = AnimationReels;
	}

	File.close();
}

// Load UI styles
void _Assets::LoadStyles(const std::string &Path) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read file
	while(!File.eof() && File.peek() != EOF) {

		std::string Name;
		std::string BackgroundColorName;
		std::string BorderColorName;
		std::string ProgramName;
		std::string TextureName;
		std::string TextureColorName;
		std::getline(File, Name, '\t');
		std::getline(File, BackgroundColorName, '\t');
		std::getline(File, BorderColorName, '\t');
		std::getline(File, ProgramName, '\t');
		std::getline(File, TextureName, '\t');
		std::getline(File, TextureColorName, '\t');

		// Check for background color
		if(BackgroundColorName != "" && Colors.find(BackgroundColorName) == Colors.end())
			throw std::runtime_error(std::string(__func__) + " unknown background color '" + BackgroundColorName + "' for style '" + Name + "'");

		// Check for border color
		if(BorderColorName != "" && Colors.find(BorderColorName) == Colors.end())
			throw std::runtime_error(std::string(__func__) + " unknown border color '" + BorderColorName + "' for style '" + Name + "'");

		// Check for texture color
		if(TextureColorName != "" && Colors.find(TextureColorName) == Colors.end())
			throw std::runtime_error(std::string(__func__) + " unknown texture color '" + TextureColorName + "' for style '" + Name + "'");

		// Find program
		if(Programs.find(ProgramName) == Programs.end())
			throw std::runtime_error(std::string(__func__) + " unknown program '" + ProgramName + "' for style '" + Name + "'");

		// Check for texture
		if(TextureName != "" && Textures.find(TextureName) == Textures.end())
			throw std::runtime_error(std::string(__func__) + " unknown texture '" + TextureName + "' for style '" + Name + "'");

		bool Stretch;
		File >> Stretch;
		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Get colors
		glm::vec4 BackgroundColor = Colors[BackgroundColorName];
		glm::vec4 BorderColor = Colors[BorderColorName];
		glm::vec4 TextureColor = Colors[TextureColorName];

		// Get textures
		const _Texture *Texture = Textures[TextureName];

		// Create style
		_Style *Style = new _Style();
		Style->Name = Name;
		Style->HasBackgroundColor = BackgroundColorName != "";
		Style->HasBorderColor = BorderColorName != "";
		Style->BackgroundColor = BackgroundColor;
		Style->BorderColor = BorderColor;
		Style->Program = Programs[ProgramName];
		Style->Texture = Texture;
		Style->TextureColor = TextureColor;
		Style->Stretch = Stretch;

		// Check for duplicates
		if(Styles.find(Name) != Styles.end())
			throw std::runtime_error(std::string(__func__) + " duplicate style '" + Name + "'");

		Styles[Name] = Style;
	}

	File.close();
}

// Load the UI xml file
void _Assets::LoadUI(const std::string &Path, bool CalculateBounds) {

	// Load file
	tinyxml2::XMLDocument Document;
	if(Document.LoadFile(Path.c_str()) != tinyxml2::XML_SUCCESS)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Load elements
	tinyxml2::XMLElement *ChildNode = Document.FirstChildElement();
	Graphics.Element = new _Element(ChildNode, nullptr);
	Graphics.Element->Alignment = LEFT_TOP;
	Graphics.Element->Active = true;
	Graphics.Element->Size = Graphics.CurrentSize;
	if(CalculateBounds)
		Graphics.Element->CalculateBounds(false);
}

// Save UI to xml
void _Assets::SaveUI(const std::string &Path) {

	// Create doc
	tinyxml2::XMLDocument Document;
	Document.InsertEndChild(Document.NewDeclaration());

	// Serialize root ui element
	Graphics.Element->SerializeElement(Document, nullptr);

	// Write file
	Document.SaveFile(Path.c_str());
}

}
