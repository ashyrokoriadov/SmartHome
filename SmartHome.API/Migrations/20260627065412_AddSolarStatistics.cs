using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SmartHome.API.Migrations
{
    /// <inheritdoc />
    public partial class AddSolarStatistics : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<int>(
                name: "ChargerState",
                table: "BatteryStates",
                type: "INTEGER",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<int>(
                name: "Error",
                table: "BatteryStates",
                type: "INTEGER",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<decimal>(
                name: "GainedEnergyToday",
                table: "BatteryStates",
                type: "TEXT",
                nullable: false,
                defaultValue: 0m);

            migrationBuilder.AddColumn<decimal>(
                name: "PanelPower",
                table: "BatteryStates",
                type: "TEXT",
                nullable: false,
                defaultValue: 0m);

            migrationBuilder.AddColumn<decimal>(
                name: "PanelVoltage",
                table: "BatteryStates",
                type: "TEXT",
                nullable: false,
                defaultValue: 0m);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "ChargerState",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "Error",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "GainedEnergyToday",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "PanelPower",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "PanelVoltage",
                table: "BatteryStates");
        }
    }
}
