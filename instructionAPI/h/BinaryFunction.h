/*
 * See the dyninst/COPYRIGHT file for copyright information.
 * 
 * We provide the Paradyn Tools (below described as "Paradyn")
 * on an AS IS basis, and do not warrant its validity or performance.
 * We reserve the right to update, modify, or discontinue this
 * software at any time.  We shall have no obligation to supply such
 * updates or modifications or any other form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#if !defined(BINARYFUNCTION_H)
#define BINARYFUNCTION_H

#include "Expression.h"
#include "Register.h"
#include "Result.h"
#include <sstream>

#if defined(_MSC_VER)
#pragma warning(disable:4251)
#endif

namespace Dyninst
{
  namespace InstructionAPI
  {
    using std::vector;

    /// A %BinaryFunction object represents a function that can combine two %Expressions and produce another %ValueComputation.
    ///
    /// For the purposes of representing a single operand of an instruction, the %BinaryFunctions of interest are addition and multiplication of
    /// integer values; this allows a %Expression to represent all addressing modes on the architectures currently
    /// supported by the %Instruction API.
    class INSTRUCTION_EXPORT BinaryFunction : public Expression
    {
		public:
			class INSTRUCTION_EXPORT funcT
			{
				public:
					funcT(std::string name) : m_name(name) 
					{
					}
					
					virtual ~funcT()
					{
					}
	
					virtual Result operator()(const Result& arg1, const Result& arg2) const = 0;
					
					std::string format() const 
					{
						return m_name;
					}
					
					typedef boost::shared_ptr<funcT> Ptr;
	
					private:
						std::string m_name;
			};
      
      
			class INSTRUCTION_EXPORT addResult : public funcT
			{
				public:
					addResult() : funcT("+") 
					{
					}
	
					virtual ~addResult()
					{
					}
					
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						return arg1 + arg2;
					}
			};
			
			class INSTRUCTION_EXPORT multResult : public funcT
			{
				public:
					multResult() : funcT("*")
					{
					}
					
					virtual ~multResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						return arg1 * arg2;
					}
			};
      
			class INSTRUCTION_EXPORT leftShiftResult : public funcT
			{
				public:
					leftShiftResult() : funcT("<<")
					{
					}
	
					virtual ~leftShiftResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						return arg1 << arg2;
					}
			};
			
			class INSTRUCTION_EXPORT rightArithmeticShiftResult : public funcT
			{
				public:
					rightArithmeticShiftResult() : funcT(">>")
					{
					}
	
					virtual ~rightArithmeticShiftResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						return arg1 >> arg2;
					}
			};
			
			class INSTRUCTION_EXPORT andResult : public funcT
			{
				public:
					andResult() : funcT("&")
					{
					}
	
					virtual ~andResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						return arg1 & arg2;
					}
			};
			
			class INSTRUCTION_EXPORT orResult : public funcT
			{
				public:
					orResult() : funcT("|")
					{
					}
	
					virtual ~orResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						return arg1 | arg2;
					}
			};
			
			class INSTRUCTION_EXPORT rightLogicalShiftResult : public funcT
			{
				public:
					rightLogicalShiftResult() : funcT("rightLogicalShift")
					{
					}
	
					virtual ~rightLogicalShiftResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						Result mask;
						switch(arg1.type)
						{
							case s8:
							case u8:mask = Result(u8, ~0);
									break;
							case s16:
							case u16:mask = Result(u16, ~0);
									break;
							case s32:
							case u32:mask = Result(u32, ~0);
									break;
							case s48:
							case u48:mask = Result(u48, ~0);
									break;
							case s64:
							case u64:mask = Result(u64, ~0);
									break;
							
						}
						
						return (arg1 >> arg2) & (mask >> arg2);
					}
			};
			
			class INSTRUCTION_EXPORT rightRotateResult : public funcT
			{
				public:
					rightRotateResult() : funcT("rightRotate")
					{
					}
	
					virtual ~rightRotateResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						Result_Type oneType;
						
						switch(arg1.type)
						{
							case s8:
							case u8:oneType = u8;
									break;
							case s16:
							case u16:oneType = u16;
									 break;
							case s32:
							case u32:oneType = u32;
									 break;
							case s48:
							case u48:oneType = u48;
									 break;
							case s64:
							case u64:oneType = u64;
									 break;
						}
						
						Result one(oneType, 1);
						Result rot = arg1 & ((one << arg2) - one);
						Result bits = Result(arg2.type, sizeof(Result_type2type<arg1.type>::type));
						
						rightLogicalShiftResult lhsRightShift;
						return lhsRightShift(arg1, arg2) | (rot << (bits - arg2));
					}
			};
			
			class INSTRUCTION_EXPORT extendResult : public funcT
			{
				public:
					extendResult() : funcT("extend")
					{
					}
	
					virtual ~extendResult()
					{
					}
	
					Result operator()(const Result& arg1, const Result& arg2) const
					{
						Result_Type rT = arg1.type;
						
						Result sixtyFour(u8, 64), bits(u8, sizeof(Result_type2type<arg2.type>::type)), max(u64, ~0);
						Result shift = sixtyFour - bits;
						
						switch(arg2.type)
						{
							case s8: 
							case s16:
							case s32:
							case s48:return ((arg1 << shift) >> shift);
									 break;
							case u8:
							case u16:
							case u32:
							case u48:return ((max >> shift) & arg1);
									 break;
						}
					}
			};
            
			/// \param arg1 first input to function
			/// \param arg2 second input to function
			/// \param result_type type of the function's result
			/// \param func implementation of the function
			///
			/// The constructor for a %BinaryFunction may take a reference-counted pointer or a plain C++ pointer to each of the
			/// child Expressions that represent its arguments.  Since the reference-counted implementation requires explicit construction,
			/// we provide overloads for all four combinations of plain and reference-counted pointers.  Note that regardless of which constructor
			/// is used, the pointers \c arg1 and \c arg2 become owned by the %BinaryFunction being constructed, and should not be deleted.
			/// They will be cleaned up when the %BinaryFunction object is destroyed.
			///
			/// The \c func parameter is a binary functor on two Results.  It should be derived from \c %funcT.  \c addResult and
			/// \c multResult, which respectively add and multiply two %Results, are provided as part of the InstructionAPI, 
			/// as they are necessary for representing address calculations.  Other \c %funcTs may be implemented by the user if desired.
			/// %funcTs have names associated with them for output and debugging purposes.  The addition and multiplication functors
			/// provided with the %Instruction API are named "+" and "*", respectively.
			BinaryFunction(Expression::Ptr arg1, Expression::Ptr arg2, Result_Type result_type,
		     funcT::Ptr func) : 
			  Expression(result_type), m_arg1(arg1), m_arg2(arg2), m_funcPtr(func)
			{
			}
              
			virtual ~BinaryFunction() 
			{
			}

			/// The %BinaryFunction version of \c eval allows the \c eval mechanism to handle complex addressing modes.  Like all of the %ValueComputation
			/// implementations, a %BinaryFunction's \c eval will return the result of evaluating the expression it represents
			/// if possible, or an empty %Result otherwise.
			/// A %BinaryFunction may have arguments that can be evaluated, or arguments that cannot.
			/// Additionally, it may have a real function pointer, or it may have a null function pointer.
			/// If the arguments can be evaluated and the function pointer is real, a result other than an empty %Result is guaranteed to be
			/// returned.  This result is cached after its initial calculation; the caching mechanism also allows
			/// outside information to override the results of the %BinaryFunction's internal computation.
			/// If the cached result exists, it is guaranteed to be returned even if the arguments or the function
			/// are not evaluable.
			virtual const Result& eval() const;
    
			/// The children of a %BinaryFunction are its two arguments.
			/// \param children Appends the children of this %BinaryFunction to \c children.
			virtual void getChildren(vector<InstructionAST::Ptr>& children) const
			{
				children.push_back(m_arg1);
				children.push_back(m_arg2);
				
				return;
			}
      
			virtual void getChildren(vector<Expression::Ptr>& children) const
			{
				children.push_back(m_arg1);
				children.push_back(m_arg2);
				
				return;
			}
      
		    /// The use set of a %BinaryFunction is the union of the use sets of its children.
		    /// \param uses Appends the use set of this %BinaryFunction to \c uses.
		    virtual void getUses(set<InstructionAST::Ptr>& uses)
		    {
				m_arg1->getUses(uses);
				m_arg2->getUses(uses);
				
				return;
			}
			
			/// \c isUsed returns true if \c findMe is an argument of this %BinaryFunction,
			/// or if it is in the use set of either argument.
			virtual bool isUsed(InstructionAST::Ptr findMe) const
			{
				return m_arg1->isUsed(findMe) || m_arg2->isUsed(findMe) 
									|| (*m_arg1 == *findMe) || (*m_arg2 == *findMe) || (*findMe == *this);
			}
			
			virtual std::string format(formatStyle how) const
			{
				std::stringstream retVal;
				if(how == memoryAccessStyle)
				{
					retVal << m_arg2->format() << "(" << m_arg1->format() << ")";
				}
				else
				{
					retVal << m_arg1->format() << " " << m_funcPtr->format() << " " << m_arg2->format();
				}
				
				return retVal.str();
			}
   		    
   		    virtual bool bind(Expression* expr, const Result& value);
			virtual void apply(Visitor* v);
			
			bool isAdd() const;
			bool isMultiply() const;
			bool isLeftShift() const;
			bool isRightArithmeticShift() const;
			bool isAndResult() const;
			bool isOrResult() const;
			bool isRightLogicalShift() const;
			bool isRightRotate() const;
			bool isExtend() const;
			
		protected:
			virtual bool isStrictEqual(const InstructionAST& rhs) const
			{
				const BinaryFunction& other(dynamic_cast<const BinaryFunction&>(rhs));
				if(*(other.m_arg1) == *m_arg1 &&
                    (*other.m_arg2) == *m_arg2)
                    return true;
                    
				if(*(other.m_arg1) == *m_arg2 &&
					(*other.m_arg2) == *m_arg1) return true;
				
				return false;
			}
  
		private:
		  Expression::Ptr m_arg1;
		  Expression::Ptr m_arg2;
		  funcT::Ptr m_funcPtr;
      
		};
	};
};


#endif // !defined(BINARYFUNCTION_H)
