using Microsoft.EntityFrameworkCore;
using SmartHome.API.Options;
using SmartHome.API.Shared;
using SmartHome.API.Shared.Influx;
using SmartHome.API.Shared.Influx.Interfaces;
using SmartHome.API.Shared.Interfaces;
using SmartHome.API.Shared.Repos;
using SmartHome.API.Shared.Repos.Interfaces;
using SmartHome.Shared.Repos.Interfaces;
using System.Reflection;

namespace SmartHome.API
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            builder.Logging.ClearProviders();
            builder.Logging.AddConsole();
            builder.Logging.SetMinimumLevel(LogLevel.Information);

            builder.Services.AddControllers();

            // Bind ConnectionStrings options from configuration
            builder.Services.Configure<ConnectionStrings>(builder.Configuration.GetSection(ConnectionStrings.Section));
            builder.Services.Configure<InfluxOptions>(builder.Configuration.GetSection(InfluxOptions.Section));

            // Use configured connection string for DbContext
            var sqliteConn = builder.Configuration.GetConnectionString("SqlLightConnection");
            builder.Services.AddDbContext<AppDbContext>(options => options.UseSqlite(sqliteConn));
            builder.Services.AddScoped<IBatteryStateRepo, ChargingControllerDataRepo>();
            builder.Services.AddScoped<ILightSensorDataRepo, LightSensorDataRepo>();
            builder.Services.AddScoped<IAirQualityDataRepoRepo, AirQualityDataRepo>();
            builder.Services.AddScoped<IAirPolutionDataRepoRepo, AirPolutionDataRepo>();
            builder.Services.AddScoped<ILogsRepo, LogsRepo>();
            builder.Services.AddScoped<IDateTimeProvider, DateTimeProvider>();
            builder.Services.AddScoped<IInfluxClient, InfluxClient>();
            builder.Services.AddScoped<IDataTypeSetter, DataTypeSetter>();

            builder.Services.AddEndpointsApiExplorer();
            builder.Services.AddSwaggerGen(
                c => 
                {
                    var xmlFile = $"{Assembly.GetExecutingAssembly().GetName().Name}.xml"; 
                    var xmlPath = Path.Combine(AppContext.BaseDirectory, xmlFile); 
                    c.IncludeXmlComments(xmlPath);
                });


            builder.WebHost.ConfigureKestrel(options => { options.ListenAnyIP(5000); options.ListenAnyIP(5001, lo => lo.UseHttps()); });

            var app = builder.Build();

            // Configure the HTTP request pipeline.

            //app.UseHttpsRedirection();

            app.UseAuthorization();

            app.MapControllers();

            app.UseSwagger();
            app.UseSwaggerUI(c => c.SwaggerEndpoint("/swagger/v1/swagger.json", "SmartHome API v1"));

            app.Run();
        }
    }
}
