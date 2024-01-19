#include "Rendering/ShaderManager.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace FalconEye
{
	std::istream& operator>>(std::istream& is, ShaderId& shader_id)
	{
		uint8_t TypeAsInt;
		is >> shader_id.NameHash >> shader_id.Permutation >> TypeAsInt;
		shader_id.Type = static_cast<EShaderType>(TypeAsInt);

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const ShaderId& shader_id)
	{
		return os << shader_id.NameHash << shader_id.Permutation << static_cast<uint8_t>(shader_id.Type);
	}

	void ShaderManager::RegisterShaderProgram(ShaderProgram_ptr Program)
	{
		std::set<ShaderId> ShaderIds;
		for (auto it : Program->Shaders)
		{
			ShaderIds.insert(it->GetShaderId());
			ShaderMap.insert({it->GetShaderId(), it});
		}
		ShaderProgramMap.insert({Program->GetId(), Program});
	}

	std::istream& operator>>(std::istream& is, ShaderProgramId& shaderProgramId)
	{
		size_t nb_shaderIds;
		is >> nb_shaderIds;
		for (int i = 0; i < nb_shaderIds; ++i)
		{
			ShaderId tmpShaderId;
			is >> tmpShaderId;
			shaderProgramId.ShaderIds.insert(tmpShaderId);
		}
		is >> shaderProgramId.ProgramHash;

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const ShaderProgramId& shaderProgramId)
	{
		os << shaderProgramId.ShaderIds.size();
		for (auto it : shaderProgramId.ShaderIds)
		{
			os << it;
		}
		return os << shaderProgramId.ProgramHash;
	}

	FalconEye::ShaderProgram_ptr ShaderManager::GetShaderProgram(ShaderProgramId Id)
	{
		auto it = ShaderProgramMap.find(Id);
		if (it != ShaderProgramMap.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void ShaderManager::FillHashMap(const std::string& Path, bool bRecursive/* = false*/)
	{
		boost::filesystem::path sp(Path);
		boost::filesystem::directory_iterator it{ sp };
		for (auto it = boost::filesystem::directory_iterator{ sp }; it != boost::filesystem::directory_iterator{}; ++it)
		{
			if (boost::filesystem::is_regular_file(*it))
			{
				if (std::string("glsl") == (*it).path().extension())
				{
					std::string stem = (*it).path().stem().string();

					// check for duplicate entry
					auto it = HashToFilenameMap.find(std::hash<std::string>{}(stem));
					if (it != HashToFilenameMap.end())
					{
						std::cerr << "Duplicate shader name already registered with ShaderManager:" << stem << ". Overriding.";
					}
					HashToFilenameMap.insert({ std::hash<std::string>{}(stem), stem });
				}
			}
			else if (bRecursive && boost::filesystem::is_directory(*it))
			{
				FillHashMap((*it).path().string(), bRecursive);
			}
		}
	}

	ShaderManager ShaderManager::Instance = ShaderManager();
	ShaderPermutationParameter::PermutationCounterType ShaderPermutationParameter::PermutationCounter;

	ShaderPermutationParameter& ShaderPermutationParameter::MakePermutationParameterForShader(const std::string& ShaderName, const std::string& ParamName)
	{
		boost::filesystem::path sp(ShaderName);
		boost::filesystem::path Stem = sp.stem();
		return MakePermutationParameterForShader(std::hash<std::string>{}(Stem.string()), ParamName);
	}

	ShaderPermutationParameter& ShaderPermutationParameter::MakePermutationParameterForShader(Hash ShaderNameHash, const std::string& ParamName)
	{
		auto& PermutationCounterShaderType = PermutationCounter[ShaderNameHash];
		auto& PermutationParameterIterator = PermutationCounterShaderType.find(ParamName);
		if (PermutationParameterIterator != PermutationCounterShaderType.end())
		{
			return PermutationParameterIterator->second;
		}
		else
		{
			auto& InsertPair = PermutationCounterShaderType.insert({ParamName, ShaderPermutationParameter(ParamName, PermutationCounterShaderType.size())});
			return InsertPair.first->second;
		}
	}

	/*ShaderPermutationParameter& ShaderPermutationParameter::MakePermutationParameterForShader(Hash ShaderNameHash, const std::string& ParamName)
	{
		auto& PermutationCounterShaderType = PermutationCounter[ShaderNameHash];
		auto& PermutationParameterIterator = PermutationCounterShaderType.find(ParamName);
		if (PermutationParameterIterator != PermutationCounterShaderType.end())
		{
			return PermutationParameterIterator->second;
		}
		else
		{
			auto& InsertPair = PermutationCounterShaderType.insert({ParamName, ShaderPermutationParameter(ParamName, PermutationCounterShaderType.size())});
			return InsertPair.first->second;
		}
	}*/

	Shader::Shader(const std::string& inShaderPath, EShaderType ShaderType, const std::vector<ShaderPermutationParameter>& inPermutationParams, bool bCompile /*= true*/)
	{
		ShaderPath = inShaderPath;
		PermutationParameters = inPermutationParams;

		boost::filesystem::path sp(ShaderPath);
		boost::filesystem::path Stem = sp.stem(); // filename without extension
		if (!Stem.string().empty())
		{
			ID = ShaderId(ShaderType, inPermutationParams, std::hash<std::string>{}(Stem.string()));
			if (bCompile)
			{
				std::string Errors;
				bIsCompiled = Compile(Errors);
				if (!bIsCompiled)
				{
					std::cerr << "Error compiling shader " << inShaderPath << " of type " << static_cast<uint8_t>(ShaderType)
					<< '\n' << Errors << '\n';
				}
			}
		}
	}

	Shader::~Shader()
	{
		glDeleteShader(ProgramID);
	}

	FalconEye::ShaderId Shader::MakeShaderID(const std::string& inShaderPath, EShaderType ShaderType, const std::vector<ShaderPermutationParameter>& inPermutationParams)
	{
		boost::filesystem::path sp(inShaderPath);
		boost::filesystem::path Stem = sp.stem(); // filename without extension
		if (!Stem.string().empty())
		{
			return ShaderId(ShaderType, inPermutationParams, std::hash<std::string>{}(Stem.string()));
		}
		return ShaderId();
	}

	// charge un fichier texte.
	static
		std::string read(const char* filename)
	{
		std::stringbuf source;
		std::ifstream in(filename);
		if (in.good() == false)
			printf("[error] loading program '%s'...\n", filename);
		else
			printf("loading program '%s'...\n", filename);

		in.get(source, 0);        // lire tout le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
		return source.str();
	}

	static
		std::string getShaderTypeDefine(EShaderType shaderType)
	{
		switch (shaderType)
		{
		case FalconEye::EShaderType::VertexShader:
			return "#define VERTEX_SHADER";
		case FalconEye::EShaderType::PixelShader:
			return "#define FRAGMENT_SHADER";
		case FalconEye::EShaderType::Num:
		default:
			return "";
		}
	}

	static
		GLenum getShaderTypeGLValue(EShaderType shaderType)
	{
		switch (shaderType)
		{
		case FalconEye::EShaderType::VertexShader:
			return  GL_VERTEX_SHADER;
		case FalconEye::EShaderType::PixelShader:
			return  GL_FRAGMENT_SHADER;
		case FalconEye::EShaderType::Num:
		default:
			return 0;
		}
	}
	// insere les definitions apres la ligne contenant #version
	static
		std::string prepare_source(std::string file, EShaderType shaderType, const std::vector<ShaderPermutationParameter>& definitions)
	{
		if (file.empty())
			return std::string();

		// un peu de gymnastique, #version doit rester sur la premiere ligne, meme si on insere des #define dans le source
		std::string source;

		// recupere la ligne #version
		std::string version;
		size_t b = file.find("#version");
		if (b != std::string::npos)
		{
			size_t e = file.find('\n', b);
			if (e != std::string::npos)
			{
				version = file.substr(0, e + 1);
				file.erase(0, e + 1);

				if (file.find("#version") != std::string::npos)
				{
					printf("[error] found several #version directives. failed.\n");
					return std::string();
				}
			}
		}
		else
		{
			printf("[error] no #version directive found. failed.\n");
			return std::string();
		}

		// reconstruit le source complet
		if (definitions.empty() == false)
		{
			source.append(version);                         // insere la version
			//source.append(definitions).append("\n");        // insere les definitions
			source.append(getShaderTypeDefine(shaderType).append("\n"));
			for (auto param : definitions)
			{
				source.append(param.GetShaderDefinition()).append("\n");
			}
			source.append(file);                            // insere le source
		}
		else
		{
			source.append(version);                         // re-insere la version (supprimee de file)
			source.assign(file);                            // insere le source
		}

		return source;
	}

	static
		GLuint compile_shader(GLuint existingShader, const GLenum shader_type, const std::string& source, std::string outLogs)
	{
		if (source.size() == 0)
		{
			return 0;
		}
		GLuint shader;

		if (existingShader != 0)
		{
			shader = existingShader;
		}
		else
		{
			shader = glCreateShader(shader_type);
		}

		if (shader != 0)
		{
			const char* sources = source.c_str();
			glShaderSource(shader, 1, &sources, NULL);
			glCompileShader(shader);

			GLint status;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		
			GLint log_size;
			GLsizei size;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

			outLogs.resize(static_cast<size_t>(log_size)+1);
			glGetShaderInfoLog(shader, log_size, &size, &outLogs[0]);
			if (status != GL_TRUE)
			{
				//glDeleteShader(shader);
			}
			return (status == GL_TRUE) ? shader : 0;
		}
		return 0;
	}

	bool Shader::Compile(std::string& OutErrors)
	{
		std::string FileContent = read(ShaderPath.c_str());
		if (FileContent.size() > 0)
		{
			std::string Logs;
			FileContent = prepare_source(FileContent, ID.GetShaderType(), PermutationParameters);
			ProgramID = compile_shader(ProgramID, getShaderTypeGLValue(ID.GetShaderType()), FileContent, Logs);
			if (ProgramID == 0)
			{
				OutErrors = std::string("Shader compile error:\n") + Logs;
			}
			else
			{
				bIsCompiled = true;
			}
		}
		else
		{
			OutErrors = std::string("Could not read shader file ") + ShaderPath;
			return false;
		}
		return true;
	}

	std::istream& operator>>(std::istream& is, Shader& shader)
	{
		GLint length = 0;
		GLenum format = 0;
		is >> shader.ID >> format >> length;
		std::vector<GLubyte> buffer(length);

		is.read(reinterpret_cast<char*>(buffer.data()), length);

		if (shader.ProgramID == 0)
		{
			shader.ProgramID = glCreateShader(shader.ID.GetGLShaderType());
;		}
		glShaderBinary(1, &shader.ProgramID, format, buffer.data(), length);
		return is;
	}

	std::ostream& operator<<(std::ostream& os, const Shader& shader)
	{
		if (shader.bIsCompiled)
		{
			GLint formats = 0;
			glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
			if (formats < 1) {
				std::cerr << "Driver does not support any binary formats." << std::endl;
				return os;
			}

			GLint length = 0;
			glGetProgramiv(shader.ProgramID, GL_PROGRAM_BINARY_LENGTH, &length);

			// Retrieve the binary code
			std::vector<GLubyte> buffer(length);
			GLenum format = 0;
			glGetProgramBinary(shader.ProgramID, length, NULL, &format, buffer.data());

			os << shader.ID << format << length;
			os.write(reinterpret_cast<char*>(buffer.data()), length);
		}
		return os;
	}

	

	PermutationId ShaderId::GetPermutationIdFromParams(const std::vector<class ShaderPermutationParameter>& PermutationParams)
	{
		PermutationId ID(0);
		for (auto param : PermutationParams)
		{
			ID |= 1 << param.GetShift();
		}
		return ID;
	}

	GLenum ShaderId::GetGLShaderType()
	{
		switch (Type)
		{
		case FalconEye::EShaderType::VertexShader:
			return GL_VERTEX_SHADER;
		case FalconEye::EShaderType::PixelShader:
			return GL_FRAGMENT_SHADER;
		case FalconEye::EShaderType::Num:
		default:
			return 0;
		}
	}

	ShaderProgram::ShaderProgram(const std::vector<Shader_ptr>& inShaders, bool bCompile)
		: Id(inShaders)
	{
		Shaders = inShaders;
		if (bCompile)
		{
			std::string Logs;
			bool bIsLinked = LinkProgram(Logs);
			if (!bIsLinked)
			{
				std::cerr << "Error linking program"
					<< '\n' << Logs << '\n';
			}
		}
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(ProgramID);
	}

	bool ShaderProgram::LinkProgram(std::string& OutLogs)
	{
		if (ProgramID == 0)
		{
			ProgramID = glCreateProgram();
		}

		for (auto shader : Shaders)
		{
			glAttachShader(ProgramID, shader->ProgramID);
		}

		glLinkProgram(ProgramID);

		// verifie les erreurs
		GLint status;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &status);

		GLint log_size;
		GLsizei size;
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &log_size);

		OutLogs.resize(static_cast<size_t>(log_size) + 1);
		glGetProgramInfoLog(ProgramID, log_size, &size, &OutLogs[0]);
		return status == GL_TRUE;
	}

	ShaderProgramId::ShaderProgramId(const std::set<ShaderId>& inShaderIds)
		: ShaderIds(inShaderIds)
	{
		GenerateProgramHashFromShaderIds();
	}

	ShaderProgramId::ShaderProgramId(const std::vector<Shader_ptr>& ShaderArray)
	{
		for (auto it : ShaderArray)
		{
			ShaderIds.insert(it->GetShaderId());
		}

		GenerateProgramHashFromShaderIds();
	}

	void ShaderProgramId::GenerateProgramHashFromShaderIds()
	{
		assert(ShaderIds.size() > 0);
		// Generate the hash value for the ShaderProgramID
		ProgramHash = std::hash<ShaderId>{}(*ShaderIds.begin());
		for (auto it = ShaderIds.begin()++; it != ShaderIds.end(); ++it)
		{
			hash_combine(ProgramHash, *it);
		}
	}

} // end namespace FalconEye