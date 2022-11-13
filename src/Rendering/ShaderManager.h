#pragma once
#include <stdint.h>
#include <string>
#include <assert.h>
#include <map>
#include <vector>
#include <memory>
#include "GL\glew.h"


namespace FalconEye
{
	using PermutationId = uint32_t;
	using Hash = std::size_t;

	enum class EShaderType : uint8_t
	{
		VertexShader,
		PixelShader,

		Num
	};

	class ShaderId
	{
	public:
		ShaderId() = default;
		ShaderId(EShaderType inType, PermutationId inPermutation, Hash inHash)
		: Type(inType)
		, Permutation(inPermutation)
		, NameHash(inHash)
		{}
		ShaderId(EShaderType inType, const std::vector<class ShaderPermutationParameter>& inPermutationParams, Hash inHash)
			: Type(inType)
			, Permutation(GetPermutationIdFromParams(inPermutationParams))
			, NameHash(inHash)
		{}
	public:
		static PermutationId GetPermutationIdFromParams(const std::vector<class ShaderPermutationParameter>& PermutationParams);
	public:
		EShaderType GetShaderType() const { return Type; }
		PermutationId GetPermutationId() const { return Permutation; }
		Hash GetNameHash() const { return NameHash; }
	private:
		EShaderType Type;
		PermutationId Permutation;
		Hash NameHash;
	};

	using PermutationCounterType = std::map<std::tuple<Hash, EShaderType>, std::map<std::string, class ShaderPermutationParameter>>;
	class ShaderPermutationParameter
	{
	public:
		ShaderPermutationParameter(const std::string& inName, uint8_t inShift) noexcept
			: ParameterName(inName)
			, Shift(inShift)
		{
			// Not enough space in PermutationID to shift this
			assert(Shift < sizeof(PermutationId));
		}
		ShaderPermutationParameter(const ShaderPermutationParameter& other) noexcept
			: ParameterName(other.ParameterName)
			, Shift(other.Shift)
		{

		}
		//ShaderPermutationParameter(ShaderPermutationParameter&&) = default;

	public:
		static ShaderPermutationParameter& MakePermutationParameterForShader(const std::string& ShaderName, EShaderType Type, const std::string& ParamName);
		static ShaderPermutationParameter& MakePermutationParameterForShader(Hash ShaderNameHash, EShaderType Type, const std::string& ParamName);

	public:
		const std::string& GetParamName() const { return ParameterName; }
		uint8_t GetShift() const { return Shift; }

		std::string GetShaderDefinition() const { return std::string("#define ").append(ParameterName); }
	private:
		
		static PermutationCounterType PermutationCounter;

	private:
		std::string ParameterName;
		uint8_t Shift;
	};

	class Shader
	{

		friend class ShaderProgram;
	public:
		Shader() = delete;
		Shader(const std::string& inShaderPath, EShaderType ShaderType, const std::vector<ShaderPermutationParameter>& inPermutationParams, bool bCompile = true);
		Shader(const Shader&) = delete;
		virtual ~Shader();

	public:
		bool Compile(std::string& OutErrors);
	private:
		ShaderId ID;
		bool bIsCompiled = false;
		GLuint ProgramID = 0;

		std::string ShaderPath;
		std::vector<ShaderPermutationParameter> PermutationParameters;
	};
	using Shader_ptr = std::shared_ptr<Shader>;

	class ShaderProgram
	{
	public:
		ShaderProgram(const std::vector<Shader_ptr>& inShaders, bool bCompile = true);
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&& other) noexcept
			: Shaders(other.Shaders)
			, bIsLinked(other.bIsLinked)
			, ProgramID(other.ProgramID)
		{
			other.ProgramID = 0;
		}
		virtual ~ShaderProgram();

	public:
		bool LinkProgram(std::string& OutLogs);
		void UseProgram() { glUseProgram(ProgramID); }
		GLuint GetProgram() { return ProgramID; }

		operator GLuint() const { return ProgramID; }
	private:
		std::vector<Shader_ptr> Shaders;
		bool bIsLinked = false;
		GLuint ProgramID = 0;
	};
	using ShaderProgram_ptr = std::shared_ptr<ShaderProgram>;

	class ShaderManager
	{
	public:
		static ShaderManager& GetInstance() { return Instance; }
	private:
		static ShaderManager Instance;

	private:
	};
} // end namespace FalconEye