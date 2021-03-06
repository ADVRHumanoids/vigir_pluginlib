//=================================================================================================
// Copyright (c) 2016, Alexander Stumpf, TU Darmstadt
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Simulation, Systems Optimization and Robotics
//       group, TU Darmstadt nor the names of its contributors may be used to
//       endorse or promote products derived from this software without
//       specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//=================================================================================================

#ifndef VIGIR_PLUGINLIB_PLUGIN_H__
#define VIGIR_PLUGINLIB_PLUGIN_H__

#include <ros/ros.h>

#include <vigir_generic_params/parameter_manager.h>

#include <vigir_pluginlib_msgs/pluginlib_msgs.h>



namespace vigir_pluginlib
{
std::string demangle(const char* name);

class Plugin
{
public:
  // typedefs
  typedef boost::shared_ptr<Plugin> Ptr;
  typedef boost::shared_ptr<const Plugin> ConstPtr;
  typedef boost::weak_ptr<Plugin> WeakPtr;
  typedef boost::weak_ptr<const Plugin> ConstWeakPtr;

  Plugin(const std::string& name, const std::string& type_class_package = std::string(), const std::string& base_class_package = std::string(), const std::string& base_class = std::string());
  virtual ~Plugin();

private:
  /**
   * @brief Internal initialization of plugin itself, e.g. setting parameter namespaces.
   * @param nh Nodehandle the plugin
   * @param params active parameter set taken from ParameterManager
   * @return true, if setup was successful
   */
  bool setup(ros::NodeHandle& nh, const vigir_generic_params::ParameterSet& params = vigir_generic_params::ParameterSet());

public:
  /**
   * @brief Loads parameters from parameter set and rosparam server (!= plugin's dedicated parameters).
   * This method will be automatically called before Plugin::initialize(...).
   * @param params active parameter set taken from ParameterManager
   * @return true, if loading parameters was successful
   */
  virtual bool loadParams(const vigir_generic_params::ParameterSet& params = vigir_generic_params::ParameterSet()) { return true; }

  /**
   * @brief Initialization of plugin specific features.
   * @param params active parameter set taken from ParameterManager
   * @return true when initialization was successful
   */
  virtual bool initialize(const vigir_generic_params::ParameterSet& params = vigir_generic_params::ParameterSet()) { return true; }

  /**
   * @brief Called after initialization of this and other plugins has been completed.
   * At this point other plugins can be used safely.
   * @param params active parameter set taken from ParameterManager
   * @return true, if post initialization was successful
   */
  virtual bool postInitialize(const vigir_generic_params::ParameterSet& params = vigir_generic_params::ParameterSet()) { return true; }

  /**
   * Used for automatically generate type ids for data types. Override _typeClass()
   * function to use custom type ids for derived data types!
   * Usage: vigir_pluginlib::Plugin::getTypeClass<MyClass>()
   * DO NOT OVERRIDE THIS METHOD! This wrapper prevents wrong usage, e.g. A::_typeClass<B>().
   */
  template <typename T>
  inline static std::string getTypeClass() { return T::template _typeClass<T>(); }
  const std::string& getTypeClass() const;

  const msgs::PluginDescription& getDescription() const;
  const std::string& getName() const;
  const std::string& getTypeClassPackage() const;
  const std::string& getBaseClassPackage() const;
  const std::string& getBaseClass() const;

  /**
   * Unique plugins (default: true) can only exist once per type_class.
   * The plugin manager will replace any plugin of same type_class with the
   * new one. If a plugin can live in coexistence with others plugins
   * with the the same type_class, override this method to return false.
   **/
  virtual bool isUnique() const { return true; }

  friend class PluginManager;

protected:
  /**
   * Used internally for automatically generate type ids for data types.
   */
  template <typename T>
  inline static std::string _typeClass(T* t = nullptr) { return demangle(t ? typeid(*t).name() : typeid(T).name()); }

  /**
   * @brief Updates plugin description
   * @param description new description
   */
  void updateDescription(const msgs::PluginDescription& description);

  /**
   * @brief Returns parameter set of plugin
   * @return parameter set of plugin
   */
  inline const vigir_generic_params::ParameterSet& getParams() const { return params_; }

  /**
   * @brief Retrieves parameter from plugin's dedicated parameter set
   * @param key key of parameter
   * @param p [out] return variable for parameter
   * @param default_val default value
   * @param ignore_warnings (default = false) When set to true, no warnings will be printed if param is not available
   * @return true when parameter was found
   */
  template<typename T>
  inline bool getParam(const std::string& key, T& p, const T& default_val, bool ignore_warnings = false) const
  {
    return params_.getParam(key, p, default_val, ignore_warnings);
  }

  /**
   * @brief Retrieves parameter from plugin's dedicated parameter set
   * @param key key of parameter
   * @param p [out] return variable for parameter
   * @param default_val default value
   * @param ignore_warnings (default = false) When set to true, no warnings will be printed if param is not available
   * @return true when parameter was found
   */
  template<typename T>
  inline bool param(const std::string& key, T& p, const T& default_val, bool ignore_warnings = false) const
  {
    return params_.param(key, p, default_val, ignore_warnings);
  }

  /**
   * @brief Retrieves parameter from plugin's dedicated parameter set
   * @param key key of parameter
   * @param default_val default value
   * @param ignore_warnings (default = false) When set to true, no warnings will be printed if param is not available
   * @return retrieved parameter if available, otherwise given default value
   */
  template<typename T>
  inline T param(const std::string& key, const T& default_val, bool ignore_warnings = false) const
  {
    return params_.param(key, default_val, ignore_warnings);
  }

  ros::NodeHandle nh_;

private:
  void setNodehandle(const ros::NodeHandle& nh);
  void setParams(const vigir_generic_params::ParameterSet& params);

  vigir_generic_params::ParameterSet params_;

  msgs::PluginDescription description_;
};
}

#endif
