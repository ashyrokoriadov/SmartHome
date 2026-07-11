using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SmartHome.API.Migrations
{
    /// <inheritdoc />
    public partial class ChangesInBatteryTableSchema : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "ChargerState",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "Current",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "Error",
                table: "BatteryStates");

            migrationBuilder.DropColumn(
                name: "GainedEnergyToday",
                table: "BatteryStates");

            migrationBuilder.RenameColumn(
                name: "Voltage",
                table: "BatteryStates",
                newName: "Value");

            migrationBuilder.RenameColumn(
                name: "PanelVoltage",
                table: "BatteryStates",
                newName: "Name");

            migrationBuilder.RenameColumn(
                name: "PanelPower",
                table: "BatteryStates",
                newName: "DataType");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.RenameColumn(
                name: "Value",
                table: "BatteryStates",
                newName: "Voltage");

            migrationBuilder.RenameColumn(
                name: "Name",
                table: "BatteryStates",
                newName: "PanelVoltage");

            migrationBuilder.RenameColumn(
                name: "DataType",
                table: "BatteryStates",
                newName: "PanelPower");

            migrationBuilder.AddColumn<int>(
                name: "ChargerState",
                table: "BatteryStates",
                type: "INTEGER",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<decimal>(
                name: "Current",
                table: "BatteryStates",
                type: "TEXT",
                nullable: false,
                defaultValue: 0m);

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
        }
    }
}
