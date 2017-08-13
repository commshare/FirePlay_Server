using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Owin;
using System.Web.Http;
using Newtonsoft.Json.Serialization;

namespace LoginServer
{
	class Startup
	{
		// Self-Hosting을 위한 세팅
		public void Configuration(IAppBuilder appBuilder)
		{
			var config = new HttpConfiguration();

			config.MapHttpAttributeRoutes();

			// 여기 아랫단은 사실 안해줘도 됨.
			config.Routes.MapHttpRoute(
				name : "DefaultApi",
				routeTemplate: "api/{controller}/{id}",
				defaults: new { id = RouteParameter.Optional }
				);
			// 왜냐면 각자의 메소드에서 라우팅을 정의해 줄 것이기 때문.

			appBuilder.UseWebApi(config);
		}
	}
}