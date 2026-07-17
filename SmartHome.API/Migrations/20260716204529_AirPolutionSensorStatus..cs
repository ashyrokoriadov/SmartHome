using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SmartHome.API.Migrations
{
    /// <inheritdoc />
    public partial class AirPolutionSensorStatus : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<int>(
                name: "SensorStatus",
                table: "AirPolutionData",
                type: "INTEGER",
                nullable: false,
                defaultValue: 0);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "SensorStatus",
                table: "AirPolutionData");
        }
    }
}
