

#ifndef __MASTER_APPLICATION_ADAPTER_H_
#define __MASTER_APPLICATION_ADAPTER_H_

using namespace System::Collections::ObjectModel;

#include <vcclr.h>

#include <opendnp3/master/IMasterApplication.h>

#include "Conversions.h"
#include "MasterConversions.h"

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{
			
			template <class Interface>
			private class MasterApplicationAdapter : public Interface
			{
			public:

				MasterApplicationAdapter(Automatak::DNP3::Interface::IMasterApplication^ proxy) : proxy(proxy)
				{}

				virtual void OnStateChange(opendnp3::LinkStatus value) override
				{
					proxy->OnStateChange((LinkStatus)value);
				}
				
				virtual void OnKeepAliveInitiated() override
				{
					proxy->OnKeepAliveInitiated();
				}

				virtual void OnKeepAliveFailure() override
				{
					proxy->OnKeepAliveFailure();
				}

				virtual void OnKeepAliveSuccess() override
				{
					proxy->OnKeepAliveSuccess();
				}

				virtual openpal::UTCTimestamp Now() override final
				{
					auto milliseconds = proxy->GetMillisecondsSinceEpoch();
					return openpal::UTCTimestamp(milliseconds);
				}

				virtual void OnReceiveIIN(const opendnp3::IINField& iin) override final
				{
					IINField ^iinField = gcnew IINField((Automatak::DNP3::Interface::LSBMask)iin.LSB, (Automatak::DNP3::Interface::MSBMask)iin.MSB);
					proxy->OnReceiveIIN(iinField);
				}

				virtual void OnTaskStart(opendnp3::MasterTaskType type, opendnp3::TaskId id) override final
				{
					proxy->OnTaskStart((MasterTaskType)type, id.GetId());
				}

				virtual void OnTaskComplete(const opendnp3::TaskInfo& info) override final
				{					
					proxy->OnTaskComplete(MasterConversions::Convert(info));
				}

				virtual bool AssignClassDuringStartup() override final
				{
					return proxy->AssignClassDuringStartup();
				}

				virtual void ConfigureAssignClassRequest(opendnp3::HeaderWriter& writer) override final
				{
					auto assignments = proxy->GetClassAssignments();
					for each(auto a in assignments)
					{
						writer.WriteHeader(Conversions::Convert(a.clazz), opendnp3::QualifierCode::ALL_OBJECTS);
						opendnp3::GroupVariationID id(a.group, a.variation);
						if (a.range.IsAllObjects())
						{
							writer.WriteHeader(id, opendnp3::QualifierCode::ALL_OBJECTS);
						}
						else
						{
							writer.WriteRangeHeader<openpal::UInt16>(opendnp3::QualifierCode::UINT16_START_STOP, id, a.range.start, a.range.stop);
						}
					}
				}

			private:				

				gcroot < Automatak::DNP3::Interface::IMasterApplication^ > proxy;
			};


		}
	}
}

#endif
