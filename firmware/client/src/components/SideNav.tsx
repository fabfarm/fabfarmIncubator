import {
  Accordion,
  AccordionBody,
  AccordionHeader,
  Button,
  Card,
  Chip,
  IconButton,
  Input,
  List,
  ListItem,
  ListItemPrefix,
  Tooltip,
  Typography,
} from "@material-tailwind/react";

import {
  BugAntIcon,
  ChartBarSquareIcon,
  ChevronDownIcon,
  PowerIcon,
  SwatchIcon,
  XMarkIcon,
} from "@heroicons/react/24/outline";
import { useEffect, useState } from "preact/hooks";
import SliderControl from "./Slider";
import DropletIcon from "./icons/DropletIcon";
import TemperatureIcon from "./icons/TemperatureIcon";
import React from "preact/compat";
import {
  fetchDataWrapper,
  useDebugMode,
  useOpStatus,
  useTargetHumidity,
  useTargetTemperature,
  useTurnAngle,
  useTurnInterval,
} from "../app";

enum INCUBATION_OP_STATUS {
  UNKNOWN = "Unknown",
  ON = "On",
  OFF = "Off",
}

export default function SideNav({ closeDrawer }: { closeDrawer: () => void }) {
  const [targetTemperature, setTargetTemperature] = useTargetTemperature();
  const [targetHumidity, setTargetHumidity] = useTargetHumidity();

  const [servoTurnAngle, setServoTurnAngle] = useTurnAngle();
  const [servoIntervalMs, setServoIntervalMs] = useTurnInterval();
  const [pidTemperatureKp, setPidTemperatureKp] = useState<number>();
  const [pidTemperatureKi, setPidTemperatureKi] = useState<number>();
  const [pidTemperatureKd, setPidTemperatureKd] = useState<number>();
  const [pidRelHumidityKp, setPidRelHumidityKp] = useState<number>();
  const [pidRelHumidityKi, setPidRelHumidityKi] = useState<number>();
  const [pidRelHumidityKd, setPidRelHumidityKd] = useState<number>();
  const [incubatorOpStatus, setIncubatorOpStatus] = useOpStatus();
  const [debugModeStatus, setDebugModeStatus] = useDebugMode();

  const [open, setOpen] = useState<number>(0);
  const [isResetDataOpened, setIsResetDataOpened] = useState<boolean>(false);

  const handleOpen = (value: number) => {
    setOpen(open === value ? 0 : value);
  };

  const handleToggleResetDataOpened = () => {
    setIsResetDataOpened(!isResetDataOpened);
  };

  const handleDataReset = () => {
    fetchDataWrapper("resetData");
    handleToggleResetDataOpened();
  };

  return (
    <Card className="top-4 left-4 h-[calc(100vh-2rem)] w-full max-w-[20rem] p-4 shadow-xl shadow-blue-gray-900/5">
      <div className="mb-2 p-4 flex items-center justify-between">
        <Typography variant="h5" color="blue-gray">
          Settings
        </Typography>
        <IconButton variant="text" color="blue-gray" onClick={closeDrawer}>
          <XMarkIcon strokeWidth={2} className="h-5 w-5" />
        </IconButton>
      </div>
      <List>
        <Accordion
          open={open === 1}
          icon={
            <ChevronDownIcon
              strokeWidth={2.5}
              className={`mx-auto h-4 w-4 transition-transform ${
                open === 1 ? "rotate-180" : ""
              }`}
            />
          }
        >
          <ListItem className="p-0" selected={open === 1}>
            <AccordionHeader
              onClick={() => handleOpen(1)}
              className="border-b-0 p-3"
            >
              <ListItemPrefix>
                <TemperatureIcon className="h-5 w-5" />
              </ListItemPrefix>
              <Typography color="blue-gray" className="mr-auto font-normal">
                Target Temperature
              </Typography>
            </AccordionHeader>
          </ListItem>
          <AccordionBody className="py-1">
            <List className="p-0 my-4">
              <SliderControl
                unit="°C"
                value={targetTemperature?.temperature ?? NaN}
                setValue={setTargetTemperature ?? (() => null)}
              ></SliderControl>
            </List>
          </AccordionBody>
        </Accordion>
        <Accordion
          open={open === 2}
          icon={
            <ChevronDownIcon
              strokeWidth={2.5}
              className={`mx-auto h-4 w-4 transition-transform ${
                open === 2 ? "rotate-180" : ""
              }`}
            />
          }
        >
          <ListItem className="p-0" selected={open === 2}>
            <AccordionHeader
              onClick={() => handleOpen(2)}
              className="border-b-0 p-3"
            >
              <ListItemPrefix>
                <DropletIcon className="h-5 w-5" />
              </ListItemPrefix>
              <Typography color="blue-gray" className="mr-auto font-normal">
                Target Rel. Humidity
              </Typography>
            </AccordionHeader>
          </ListItem>
          <AccordionBody className="py-1">
            <List className="p-0 my-4">
              <SliderControl
                unit="%"
                value={targetHumidity?.humidity ?? NaN}
                setValue={setTargetHumidity ?? (() => null)}
              ></SliderControl>
            </List>
          </AccordionBody>
        </Accordion>
        <Accordion
          open={open === 3}
          icon={
            <ChevronDownIcon
              strokeWidth={2.5}
              className={`mx-auto h-4 w-4 transition-transform ${
                open === 3 ? "rotate-180" : ""
              }`}
            />
          }
        >
          <ListItem className="p-0" selected={open === 3}>
            <AccordionHeader
              onClick={() => handleOpen(3)}
              className="border-b-0 p-3"
            >
              <ListItemPrefix>
                <SwatchIcon className="h-5 w-5" />
              </ListItemPrefix>
              <Typography color="blue-gray" className="mr-auto font-normal">
                Servo
              </Typography>
            </AccordionHeader>
          </ListItem>
          <AccordionBody className="py-1">
            <List className="flex flex-col gap-8">
              <SliderControl
                min={0}
                max={360}
                label="Servo turn angle"
                unit="°"
                value={servoTurnAngle}
                setValue={setServoTurnAngle}
              ></SliderControl>
              <Input
                inputProps={{
                  type: "number",
                  value: servoIntervalMs,
                  onChange: (e: React.ChangeEvent<HTMLInputElement>) =>
                    setServoIntervalMs(parseInt(e.currentTarget.value)),
                }}
                label="Servo turn interval: (ms)"
                variant="outlined"
              />
            </List>
          </AccordionBody>
        </Accordion>

        <Accordion
          open={open === 4}
          icon={
            <ChevronDownIcon
              strokeWidth={2.5}
              className={`mx-auto h-4 w-4 transition-transform ${
                open === 4 ? "rotate-180" : ""
              }`}
            />
          }
        >
          <ListItem className="p-0" selected={open === 4}>
            <AccordionHeader
              onClick={() => handleOpen(4)}
              className="border-b-0 p-3"
            >
              <ListItemPrefix>
                <ChartBarSquareIcon className="h-5 w-5" />
              </ListItemPrefix>
              <Typography color="blue-gray" className="mr-auto font-normal">
                PID
              </Typography>
            </AccordionHeader>
          </ListItem>
          <AccordionBody className="py-1">
            <List className="p-0">
              <Input
                inputProps={{
                  type: "number",
                  step: 0.01,
                  value: { pidTemperatureKp },
                  onChange: (event: React.ChangeEvent<HTMLInputElement>) =>
                    setPidTemperatureKp(Number(event.currentTarget.value)),
                }}
                label="Temperature Kp:"
                variant="outlined"
              />
              <Input
                inputProps={{
                  type: "number",
                  step: 0.01,
                  value: { pidTemperatureKi },
                  onChange: (event: React.ChangeEvent<HTMLInputElement>) =>
                    setPidTemperatureKi(Number(event.currentTarget.value)),
                }}
                label="Temperature Ki:"
                variant="outlined"
              />
              <Input
                inputProps={{
                  type: "number",
                  step: 0.01,
                  value: { pidTemperatureKd },
                  onChange: (event: React.ChangeEvent<HTMLInputElement>) =>
                    setPidTemperatureKd(Number(event.currentTarget.value)),
                }}
                label="Temperature Kd:"
                variant="outlined"
              />
              <Input
                inputProps={{
                  type: "number",
                  step: 0.01,
                  value: { pidRelHumidityKp },
                  onChange: (event: React.ChangeEvent<HTMLInputElement>) =>
                    setPidRelHumidityKp(Number(event.currentTarget.value)),
                }}
                label="Rel. Humidity Kp:"
                variant="outlined"
              />
              <Input
                inputProps={{
                  type: "number",
                  step: 0.01,
                  value: { pidRelHumidityKi },
                  onChange: (event: React.ChangeEvent<HTMLInputElement>) =>
                    setPidRelHumidityKi(Number(event.currentTarget.value)),
                }}
                label="Rel. Humidity Ki:"
                variant="outlined"
              />
              <Input
                inputProps={{
                  type: "number",
                  step: 0.01,
                  value: { pidRelHumidityKd },
                  onChange: (event: React.ChangeEvent<HTMLInputElement>) =>
                    setPidRelHumidityKd(Number(event.currentTarget.value)),
                }}
                label="Rel. Humidity Kd:"
                variant="outlined"
              />
              <Button disabled className="disabled:cursor-not-allowed">
                Auto-tune PID Params
              </Button>
            </List>
          </AccordionBody>
        </Accordion>

        <Accordion
          open={open === 5}
          icon={
            <ChevronDownIcon
              strokeWidth={2.5}
              className={`mx-auto h-4 w-4 transition-transform ${
                open === 5 ? "rotate-180" : ""
              }`}
            />
          }
        >
          <ListItem className="p-0" selected={open === 5}>
            <AccordionHeader
              onClick={() => handleOpen(5)}
              className="border-b-0 p-3"
            >
              <ListItemPrefix>
                <PowerIcon className="h-5 w-5" />
              </ListItemPrefix>
              <Typography color="blue-gray" className="mr-auto font-normal">
                Operation
              </Typography>
            </AccordionHeader>
          </ListItem>
          <AccordionBody className="py-1">
            <List className="flex">
              <Button onClick={() => setIncubatorOpStatus(!incubatorOpStatus)}>
                Toggle Operation
              </Button>
              <div className="flex gap-4">
                <Typography className="font-semibold">Status:</Typography>
                <Chip
                  color="gray"
                  value={incubatorOpStatus ?? INCUBATION_OP_STATUS.UNKNOWN}
                  className="w-full"
                />
              </div>
            </List>
          </AccordionBody>
        </Accordion>

        <Accordion
          open={open === 6}
          icon={
            <ChevronDownIcon
              strokeWidth={2.5}
              className={`mx-auto h-4 w-4 transition-transform ${
                open === 6 ? "rotate-180" : ""
              }`}
            />
          }
        >
          <ListItem className="p-0" selected={open === 6}>
            <AccordionHeader
              onClick={() => handleOpen(6)}
              className="border-b-0 p-3"
            >
              <ListItemPrefix>
                <BugAntIcon className="h-5 w-5" />
              </ListItemPrefix>
              <Typography color="blue-gray" className="mr-auto font-normal">
                Debugging
              </Typography>
            </AccordionHeader>
          </ListItem>
          <AccordionBody className="py-1">
            <List className="flex flex-col gap-4">
              <Button onClick={() => setDebugModeStatus(!debugModeStatus)}>
                Toggle Debug Mode
              </Button>
              <div className="flex gap-4">
                <Typography className="font-semibold">Status:</Typography>
                <Chip
                  color="gray"
                  value={debugModeStatus ?? "Unknown"}
                  className="w-full"
                />
              </div>
              <Button color="orange" onClick={handleToggleResetDataOpened}>
                Reset Data
              </Button>
              {isResetDataOpened && (
                <>
                  <Typography className="text-center font-semibold">
                    Are you sure?
                  </Typography>
                  <Tooltip
                    position="top"
                    className="z-[99999999999]"
                    content="Use this when you want to start a
                    fresh incubation.
                    "
                  >
                    <Button color="red" onClick={handleDataReset}>
                      Yes
                    </Button>
                  </Tooltip>
                </>
              )}
            </List>
          </AccordionBody>
        </Accordion>
      </List>
    </Card>
  );
}
