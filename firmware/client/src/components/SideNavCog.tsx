import { ChevronLeftIcon } from "@heroicons/react/24/outline";
import {
  IconButton,
  SpeedDial,
  SpeedDialHandler,
} from "@material-tailwind/react";

type SideNavCogProps = {
  handleSideNavCogClick: () => void;
};

export default function SideNavCog({ handleSideNavCogClick }: SideNavCogProps) {
  return (
    <div className="relative w-full h-80" onClick={handleSideNavCogClick}>
      <div className="fixed bottom-8 right-8">
        {/* @ts-ignore */}
        <SpeedDial>
          <SpeedDialHandler>
            <IconButton size="lg" className="rounded-full">
              <ChevronLeftIcon className="h-5 w-5 transition-transform group-hover:rotate-180" />
            </IconButton>
          </SpeedDialHandler>
        </SpeedDial>
      </div>
    </div>
  );
}
