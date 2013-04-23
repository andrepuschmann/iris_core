/**
 * \file TemplatePhyComponent.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * \section LICENSE
 *
 * This file is part of the Iris Project.
 *
 * Iris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Iris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 * \section DESCRIPTION
 *
 * Base class for templated Phy Components + template helper functions.
 */

#ifndef IRISAPI_TEMPLATEPHYCOMPONENT_H_
#define IRISAPI_TEMPLATEPHYCOMPONENT_H_

#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/utility.hpp>

#include <irisapi/PhyComponent.h>

namespace iris
{

/** Base class for template PhyComponents.
 *
 * Template PhyComponents can operate on multiple input and/or output data types.
 */
template <class Comp>
class TemplatePhyComponent : public PhyComponent
{
public:
  TemplatePhyComponent(std::string name, std::string type, std::string description, std::string author, std::string version)
    : PhyComponent(name, type, description, author, version) {}

  virtual ~TemplatePhyComponent() {}

  virtual void initialize()
  {
    LOG(LERROR) << "initialise() needs to be implemented by " << getType() << "Impl class";
  }
  virtual void process()
  {
    LOG(LERROR) << "process() needs to be implemented by " << getType() << "Impl class";
  }
  virtual void destroy()
  {
    LOG(LERROR) << "destroy() needs to be implemented by " << getType() << "Impl class";
  }

  /** Sets up input and output types.
   *
   * \param inputTypes Vector of input type identifiers, each element represents a port
   * \param outputTypes Vector of output type identifiers, each element represents a port
   * \return A PhyComponent* with the input and output correctly setup, i.e., an instance of
   * CompImp with the correct template parameters according to inputTypes & outputTypes.
   */
  virtual PhyComponent* setupIO(const std::vector<int>& inputTypes, const std::vector<int>& outputTypes)
  {
    return registerTemplateComponent(inputTypes, outputTypes);
  }

protected:
  /** Called by derived class's setupIO() function as a helper for figuring out which types a
   *  templated component should be instantiated with.
   *
   * \param inputTypes Vector of input type identifiers, each element represents a port
   * \param outputTypes Vector of output type identifiers, each element represents a port
   * \return A newly instantiated object of a templated component with the appropriate type
   * parameters.
   */
  PhyComponent* registerTemplateComponent(const std::vector<int>& inputTypes, const std::vector<int>& outputTypes);

};


namespace detail
{
  namespace mpl = boost::mpl;

  /** Meta-functor to find if the template type is a sequence of sequences.
   *
   * Examples are boost::mpl::vector< boost::mpl::vector<int, float>,
   * boost::mpl::vector<double, char> > isMultiSequence<T>::value evaluates to true or false.
   */
  template <class T, class Enabled = void>
  struct isMultiSequence :  public mpl::false_
  {};

  // specialisation for multisequences, enabled if first element of T is a sequence
  template <class T>
  struct isMultiSequence<T, typename boost::enable_if<
                           mpl::is_sequence<typename mpl::front<T>::type>
              >::type > : public mpl::true_
  {};


  /** Helper class for iterating through all supported input and output types of a given
   * Component, build up template parameters for the current configuration, and create a new
   * Template component with the correct template parameters.
   *
   * Here is how the algorithm works in pseudo-code:
   * \code
   * // Derived : A component derived from this TemplatePhyComponent, with fields supportedInputTypes,
   * //       supportedOutputTypes, and static method createInstance<Tin,Tout>(comp)
   * iIn = empty_type_vector;
   * iOut = empty_type_vector;
   * In = Derived::supportedInputTypes;
   * Out = Derived::supportedOutputTypes;
   * // inVect: std::vector of type identifiers for current input types on each port
   * // outVect: std::vector of type identifiers for current output types on each port
   * // inAcc, outAcc: type vectors which accumulate the correct types for each port
   *
   * while (true)
   * {
   *  if (empty(iIn) && !empty(In))
   *  {
   *    iIn.push_back(In.front());
   *    In.pop(front);
   *  }
   *  if (!empty(iIn))
   *  {
   *    if (inVect.front() == iIn.front().typeId)
   *    {
   *      inAcc.push_back(iIn.front());
   *      iIn.clear();
   *      inVect.pop_front();
   *    }
   *    else
   *      iIn.pop_front();
   *  }
   *  if (empty(iIn) && empty(In) && empty(iOut) && !empty(Out))
   *  {
   *    iOut.push_back(Out.front());
   *    Out.pop_front();
   *  }
   *  if (empty(iIn) && empty(In) && !empty(iOut) )
   *  {
   *    if (outVect.front() == iOut.front().typeId)
   *    {
   *      outAcc.push_back(iOut.front());
   *      iOut.clear();
   *      outVect.pop_front();
   *    }
   *    else
   *      iOut.pop_front();
   *  }
   *  if (empty(iIn) && empty(In) && empty(iOut) && empty(Out) )
   *  {
   *    return new Derived::createInstance<inAcc, outAcc)(currentComponent);
   *    break;
   *  }
   * }
   * \endcode
   *
   */
  template <class Derived,
        class In,
        class Out,
        class iIn,
        class iOut,
        class inAcc = mpl::vector<>,
        class outAcc = mpl::vector<>,
        class Enabled = void>
  struct TemplateHelper
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, const PhyComponent* comp)
    {
      // we should never get here
      assert(false);
      return NULL;
    }
  };


  /// specialisation, enable if empty(iIn) && !empty(In)
  template <class Derived,
        class In,
        class Out,
        class iIn,
        class iOut,
        class inAcc,
        class outAcc>
  struct TemplateHelper<Derived, In, Out, iIn, iOut, inAcc, outAcc,
              typename boost::enable_if< mpl::and_< mpl::empty<iIn>, mpl::not_< mpl::empty<In> > > >::type >
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, const PhyComponent* comp)
    {
      typedef typename mpl::front<In>::type newiIn;
      typedef typename mpl::pop_front<In>::type newIn;
      return TemplateHelper<Derived, newIn, Out, newiIn, iOut, inAcc, outAcc>::EXEC(inTypes, outTypes, comp);
    }
  };

  /// specialisation enabled if !empty(iIn)
  template <class Derived,
        class In,
        class Out,
        class iIn,
        class iOut,
        class inAcc,
        class outAcc>
  struct TemplateHelper<Derived, In, Out, iIn, iOut, inAcc, outAcc,
              typename boost::enable_if< mpl::not_< mpl::empty<iIn> > >::type >
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, const PhyComponent* comp)
    {
      typedef typename mpl::front<iIn>::type curType;
      typedef typename mpl::pop_front<iIn>::type newiIn;

      if (!inTypes.empty())
      {
        if (TypeInfo<curType>::identifier == inTypes.front() )
        {
          inTypes.erase(inTypes.begin() );
          typedef typename mpl::push_back<inAcc, curType>::type newinAcc;
          return TemplateHelper<Derived, In, Out, mpl::vector<>, iOut, newinAcc, outAcc>::EXEC(inTypes, outTypes, comp);
        }
        else
        {
          return TemplateHelper<Derived, In, Out, newiIn, iOut, inAcc, outAcc>::EXEC(inTypes, outTypes, comp);
        }
      }
      else
      {
        return TemplateHelper<Derived, In, Out, newiIn, iOut, inAcc, outAcc>::EXEC(inTypes, outTypes, comp);
      }

    }
  };

  /// specialisation, enabled if empty(iIn) &&& empty(In) && empty(iOut) && !empty(Out)
  template <class Derived,
        class In,
        class Out,
        class iIn,
        class iOut,
        class inAcc,
        class outAcc>
  struct TemplateHelper<Derived, In, Out, iIn, iOut, inAcc, outAcc,
              typename boost::enable_if<
               mpl::and_< mpl::empty<iIn>,
                    mpl::empty<In>,
                    mpl::empty<iOut>,
                    mpl::not_<mpl::empty<Out> > > >::type >
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, const PhyComponent* comp)
    {
      typedef typename mpl::front<Out>::type newiOut;
      typedef typename mpl::pop_front<Out>::type newOut;
      return TemplateHelper<Derived, In, newOut, iIn, newiOut, inAcc, outAcc>::EXEC(inTypes, outTypes, comp);
    }
  };

  /// specialisation, enabled if empty(In) && empty(iIn) && !empty(iOut)
  template <class Derived,
        class In,
        class Out,
        class iIn,
        class iOut,
        class inAcc,
        class outAcc>
  struct TemplateHelper<Derived, In, Out, iIn, iOut, inAcc, outAcc,
              typename boost::enable_if<
               mpl::and_< mpl::empty<iIn>,
                    mpl::empty<In>,
                    mpl::not_<mpl::empty<iOut> >  > >::type >
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, const PhyComponent* comp)
    {
      typedef typename mpl::front<iOut>::type curType;
      typedef typename mpl::pop_front<iOut>::type newiOut;

      if (!outTypes.empty())
      {
        if (TypeInfo<curType>::identifier == outTypes.front())
        {
          outTypes.erase(outTypes.begin() );
          typedef typename mpl::push_back<outAcc, curType>::type newoutAcc;
          return TemplateHelper<Derived, In, Out, iIn, mpl::vector<>, inAcc, newoutAcc>::EXEC(inTypes, outTypes, comp);
        }
        else
        {
          return TemplateHelper<Derived, In, Out, iIn, newiOut, inAcc, outAcc>::EXEC(inTypes, outTypes, comp);
        }
      }
      else
      {
        return TemplateHelper<Derived, In, Out, iIn, newiOut, inAcc, outAcc>::EXEC(inTypes, outTypes, comp);
      }

    }
  };

  /// all empty => ready
  template <class Derived,
        class In,
        class Out,
        class iIn,
        class iOut,
        class inAcc,
        class outAcc>
  struct TemplateHelper<Derived, In, Out, iIn, iOut, inAcc, outAcc,
              typename boost::enable_if<
               mpl::and_< mpl::empty<iIn>,
                    mpl::empty<In>,
                    mpl::empty<iOut>,
                    mpl::empty<Out> > >::type >
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, const PhyComponent* comp)
    {
      return Derived::template createInstance<inAcc, outAcc>(comp);
    }
  };


  /** This is a helper meta-functor, which sets up the appropriate template parameters and calls
   *  TemplateHelper<...>::EXEC.
   *
   *  The main reason is to allow DerivedComponent::supportedInputTypes to be a singe sequence of types
   *  in case of a component with only one input port, instead of always requiring a type vector of
   *  type vectors (isMultiSequence<T>::value returns true for this case). The same is true for
   *  supportedOutputTypes. Specialisations of this templates are available for all combinations of
   *  input/output types, i.e., for isMultiSequence<supportedInputTypes> && !isMultiSequence<supportedOutputTypes>,
   *  etc. The default case is that both are multi-sequences.
   */
  template <class DerivedComponent, class Enabled = void>
  struct TemplateHelperCaller
  {
    static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, PhyComponent* comp)
    {
      // defaulting to both multisequence
      return TemplateHelper<DerivedComponent,
                  typename DerivedComponent::supportedInputTypes,
                  typename DerivedComponent::supportedOutputTypes,
                  mpl::vector<>,
                  mpl::vector<>,
                  mpl::vector<>,
                  mpl::vector<> >::EXEC(inTypes, outTypes, comp);

    }
  };

  /// Specialisation if DerivedComponent::supportedInputTypes is not a multisequence
  template <class DerivedComponent>
    struct TemplateHelperCaller<DerivedComponent,
    typename boost::enable_if< mpl::and_< mpl::not_<detail::isMultiSequence< typename DerivedComponent::supportedInputTypes > >,
                        detail::isMultiSequence< typename DerivedComponent::supportedOutputTypes >
                     > >::type >
    {
      static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, PhyComponent* comp)
      {
        // defaulting to both multisequence
        return TemplateHelper<DerivedComponent,
                    mpl::vector<>,
                    typename DerivedComponent::supportedOutputTypes,
                    typename DerivedComponent::supportedInputTypes,
                    mpl::vector<>,
                    mpl::vector<>,
                    mpl::vector<> >::EXEC(inTypes, outTypes, comp);

      }
    };

  /** Specialisation if neither DerivedComponent::supportedInputTypes or
   * DerivedComponent::supportedOutputTypes are multisequences.
   */
  template <class DerivedComponent>
    struct TemplateHelperCaller<DerivedComponent,
    typename boost::enable_if< mpl::and_< mpl::not_<detail::isMultiSequence< typename DerivedComponent::supportedInputTypes > >,
                        mpl::not_<detail::isMultiSequence< typename DerivedComponent::supportedOutputTypes > >
                     > >::type >
    {
      static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, PhyComponent* comp)
      {
        // defaulting to both multisequence
        return TemplateHelper<DerivedComponent,
                    mpl::vector<>,
                    mpl::vector<>,
                    typename DerivedComponent::supportedInputTypes,
                    typename DerivedComponent::supportedOutputTypes,
                    mpl::vector<>,
                    mpl::vector<> >::EXEC(inTypes, outTypes, comp);

      }
    };

  /// Specialisation enabled if DerivedComponent::supportedOutputTypes is not a multisequence
  template <class DerivedComponent>
    struct TemplateHelperCaller<DerivedComponent,
    typename boost::enable_if< mpl::and_< detail::isMultiSequence< typename DerivedComponent::supportedInputTypes > ,
                        mpl::not_<detail::isMultiSequence< typename DerivedComponent::supportedOutputTypes > >
                     > >::type >
    {
      static PhyComponent* EXEC(std::vector<int>& inTypes, std::vector<int>& outTypes, PhyComponent* comp)
      {
        // defaulting to both multisequence
        return TemplateHelper<DerivedComponent,
                    typename DerivedComponent::supportedInputTypes,
                    mpl::vector<>,
                    mpl::vector<>,
                    typename DerivedComponent::supportedOutputTypes,
                    mpl::vector<>,
                    mpl::vector<> >::EXEC(inTypes, outTypes, comp);

      }
    };
}

template <class Comp>
inline PhyComponent*
TemplatePhyComponent<Comp>::registerTemplateComponent(const std::vector<int>& inputTypes, const std::vector<int>& outputTypes)
{
   using namespace detail;

   // create temporary to allow modification without affecting the parameters
   std::vector<int> tmpIn(inputTypes);
   std::vector<int> tmpOut(outputTypes);

   return TemplateHelperCaller<Comp>::EXEC(tmpIn, tmpOut, this);
}

} // namespace iris
#endif // IRISAPI_TEMPLATEPHYCOMPONENT_H_
