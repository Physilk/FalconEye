#include "Rendering/ShaderManager.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace FalconEye
{
	ShaderManager ShaderManager::Instance = ShaderManager();
	PermutationCounterType ShaderPermutationParameter::PermutationCounter;

	ShaderPermutationParameter& ShaderPermutationParameter::MakePermutationParameterForShader(const std::string& ShaderName, EShaderType Type, const std::string& ParamName)
	{
		return MakePermutationParameterForShader(std::hash<std::string>{}(ShaderName), Type, ParamName);
	}

	ShaderPermutationParameter& ShaderPermutationParameter::MakePermutationParameterForShader(Hash ShaderNameHash, EShaderType Type, const std::string& ParamName)
	{
		auto& PermutationCounterShaderType = PermutationCounter[{ ShaderNameHash , Type}];
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

	PermutationId ShaderId::GetPermutationIdFromParams(const std::vector<class ShaderPermutationParameter>& PermutationParams)
	{
		PermutationId ID(0);
		for (auto param : PermutationParams)
		{
			ID |= 1 << param.GetShift();
		}
		return ID;
	}

	ShaderProgram::ShaderProgram(const std::vector<Shader_ptr>& inShaders, bool bCompile)
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

} // end namespace FalconEye