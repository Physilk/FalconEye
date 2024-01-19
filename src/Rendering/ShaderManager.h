#pragma once
#include <stdint.h>
#include <string>
#include <assert.h>
#include <map>
#include <vector>
#include <memory>
#include "glcore.h"
#include <set>


namespace FalconEye
{
	using PermutationId = uint32_t;
	using Hash = std::size_t;

	//putting this here for now
	template <class T>
	inline void hash_combine(Hash& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

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

		bool operator==(const ShaderId& other) const
		{
			return !(*this!=other);
		}

		bool operator!=(const ShaderId& other) const
		{
			return NameHash != other.NameHash || Type != other.Type || Permutation != other.Permutation;
		}

		bool operator<(const ShaderId& other) const
		{
			return NameHash < other.NameHash || Permutation < other.Permutation || Type < other.Type;
		}

		friend std::istream& operator>> (std::istream& is, ShaderId& shader_id);
		friend std::ostream& operator<< (std::ostream& os, const ShaderId& shader_id);

	public:
		static PermutationId GetPermutationIdFromParams(const std::vector<class ShaderPermutationParameter>& PermutationParams);
	public:
		EShaderType GetShaderType() const { return Type; }
		PermutationId GetPermutationId() const { return Permutation; }
		Hash GetNameHash() const { return NameHash; }

		GLenum GetGLShaderType();
	private:
		EShaderType Type;
		PermutationId Permutation;
		Hash NameHash;
	};

	class ShaderPermutationParameter
	{
		using PermutationCounterType = std::map<Hash, std::map<std::string, class ShaderPermutationParameter>>;
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

		static ShaderPermutationParameter& MakePermutationParameterForShader(const std::string& ShaderName, const std::string& ParamName);
		static ShaderPermutationParameter& MakePermutationParameterForShader(Hash ShaderNameHash, const std::string& ParamName);

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
		static ShaderId MakeShaderID(const std::string& inShaderPath, EShaderType ShaderType, const std::vector<ShaderPermutationParameter>& inPermutationParams);

	public:
		bool Compile(std::string& OutErrors);
		const ShaderId& GetShaderId() const { return ID;}

		operator GLuint() const { return ProgramID; }

		friend std::istream& operator>> (std::istream& is, Shader& shader);
		friend std::ostream& operator<< (std::ostream& os, const Shader& shader);

		//std::istream& operator>> (std::istream& is);
		//std::ostream& operator<< (std::ostream& os);
	private:
		ShaderId ID;
		bool bIsCompiled = false;
		GLuint ProgramID = 0;

		std::string ShaderPath;
		std::vector<ShaderPermutationParameter> PermutationParameters;
	};
	using Shader_ptr = std::shared_ptr<Shader>;

	class ShaderProgramId {
	public:
		ShaderProgramId(const std::set<ShaderId>& inShaderIds);
		ShaderProgramId(const std::vector<Shader_ptr>& ShaderArray);
		ShaderProgramId(const ShaderProgramId& other)
			: ShaderIds(other.ShaderIds)
			, ProgramHash(other.ProgramHash)
		{}

		// Equality comparison operator for ShaderProgramID
		bool operator==(const ShaderProgramId& other) const {
			return !(*this != other);
		}

		bool operator!=(const ShaderProgramId& other) const
		{
			return ProgramHash != other.ProgramHash;
		}

		bool operator<(const ShaderProgramId& other) const
		{
			return ProgramHash < other.ProgramHash;
		}
	private:
		void GenerateProgramHashFromShaderIds();
	public:
		friend std::istream& operator>> (std::istream& is, ShaderProgramId& shader);
		friend std::ostream& operator<< (std::ostream& os, const ShaderProgramId& shader);
	private:
		std::set<ShaderId> ShaderIds;
		Hash ProgramHash;
	};

	class ShaderProgram
	{
		friend class ShaderManager;
	public:
		ShaderProgram(const std::vector<Shader_ptr>& inShaders, bool bCompile = true);
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&& other) noexcept
			: Shaders(other.Shaders)
			, bIsLinked(other.bIsLinked)
			, ProgramID(other.ProgramID)
			, Id(other.Id)
		{
			other.ProgramID = 0;
		}
		virtual ~ShaderProgram();

	public:
		bool LinkProgram(std::string& OutLogs);
		void UseProgram() { glUseProgram(ProgramID); }
		GLuint GetProgram() const { return ProgramID; }

		ShaderProgramId GetId() const { return Id; }
		operator GLuint() const { return ProgramID; }
	private:
		std::vector<Shader_ptr> Shaders;
		bool bIsLinked = false;
		GLuint ProgramID = 0;

		ShaderProgramId Id;
	};

	using ShaderProgram_ptr = std::shared_ptr<ShaderProgram>;

	class ShaderManager
	{
	public:
		static ShaderManager& GetInstance() { return Instance; }

		void RegisterShaderProgram(ShaderProgram_ptr Program);
		ShaderProgram_ptr GetShaderProgram(ShaderProgramId Id);
	private:
		// TODO: Configure shaders from LUA
		//LuaIntf::LuaContext LuaContext; 

		//ShaderProgram_ptr GetShaderProgram(const std::set<ShaderId>& ShaderIds);
		//Shader_ptr GetShader(const ShaderId& ShaderId);
		void FillHashMap(const std::string& Path, bool bRecursive = false);
	private:
		static ShaderManager Instance;
		
		using ShaderContainerType = std::map<ShaderId, Shader_ptr>;
		using ProgramContainerType = std::map<ShaderProgramId, ShaderProgram_ptr>;
		using HashToFilenameType = std::map<Hash, std::string>;

		ShaderContainerType ShaderMap;
		ProgramContainerType ShaderProgramMap;
		HashToFilenameType HashToFilenameMap;
	};
} // end namespace FalconEye

template <> struct std::hash<FalconEye::ShaderId>
{
	FalconEye::Hash operator()(const FalconEye::ShaderId& x) const
	{
		FalconEye::Hash tmp_hash = x.GetNameHash();
		FalconEye::hash_combine<FalconEye::EShaderType>(tmp_hash, x.GetShaderType());
		FalconEye::hash_combine<FalconEye::PermutationId>(tmp_hash, x.GetPermutationId());
		return tmp_hash;
	}
};