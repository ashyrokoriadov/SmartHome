using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SmartHome.API.Migrations
{
    /// <inheritdoc />
    public partial class AirQualityData : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "TemperatureData");

            migrationBuilder.CreateTable(
                name: "AirPolutionData",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "TEXT", nullable: false),
                    ECO2 = table.Column<int>(type: "INTEGER", nullable: false),
                    TVOC = table.Column<int>(type: "INTEGER", nullable: false),
                    AQI = table.Column<int>(type: "INTEGER", nullable: false),
                    CorrelationId = table.Column<Guid>(type: "TEXT", nullable: false),
                    Timestamp = table.Column<DateTime>(type: "TEXT", nullable: false),
                    Location = table.Column<string>(type: "TEXT", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_AirPolutionData", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "AirQualityData",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "TEXT", nullable: false),
                    Temperature = table.Column<decimal>(type: "TEXT", nullable: false),
                    TemperatureExternal = table.Column<decimal>(type: "TEXT", nullable: false),
                    Humidity = table.Column<decimal>(type: "TEXT", nullable: false),
                    Pressure = table.Column<decimal>(type: "TEXT", nullable: false),
                    Altitude = table.Column<decimal>(type: "TEXT", nullable: false),
                    CorrelationId = table.Column<Guid>(type: "TEXT", nullable: false),
                    Timestamp = table.Column<DateTime>(type: "TEXT", nullable: false),
                    Location = table.Column<string>(type: "TEXT", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_AirQualityData", x => x.Id);
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "AirPolutionData");

            migrationBuilder.DropTable(
                name: "AirQualityData");

            migrationBuilder.CreateTable(
                name: "TemperatureData",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "TEXT", nullable: false),
                    CorrelationId = table.Column<Guid>(type: "TEXT", nullable: false),
                    Location = table.Column<string>(type: "TEXT", nullable: false),
                    Temperature = table.Column<decimal>(type: "TEXT", nullable: false),
                    Timestamp = table.Column<DateTime>(type: "TEXT", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_TemperatureData", x => x.Id);
                });
        }
    }
}
